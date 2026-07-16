#include "DronePhysics.h"
#include <chrono>
#include <cmath>
#include <thread>

DronePhysics::DronePhysics(const MissionConfig& cfg)
    : cfg_(cfg),
      dt_(cfg.physicsTimeStep),
      // Прискорення з наявних параметрів: v² = 2·a·accelPath -> a = attackSpeed²/(2·accelPath).
      accel_(cfg.accelPath > 0.0f ? (cfg.attackSpeed * cfg.attackSpeed) / (2.0f * cfg.accelPath)
                                  : cfg.attackSpeed),
      currentCmd_{STOPPED, 0.0f},
      pos_(cfg.startPos),
      direction_(cfg.initialDir),
      speed_(0.0f),
      timeSecSinceStart_(0.0f) {
    telemetry_ = {pos_, {0.0f, 0.0f}, 0.0f, direction_};
}

void DronePhysics::integrate(float dt) {
    // Забираємо всі команди, що накопичились; лишається остання (актуальний режим).
    while (auto cmd = commands_.tryPop()) {
        currentCmd_ = *cmd;
    }

    switch (currentCmd_.state) {
        case STOPPED:
            speed_ = 0.0f;
            break;
        case ACCELERATING:
            speed_ = std::min(cfg_.attackSpeed, speed_ + accel_ * dt);
            break;
        case MOVING:
            speed_ = cfg_.attackSpeed;
            break;
        case DECELERATING:
            speed_ = std::max(0.0f, speed_ - accel_ * dt);
            break;
        case TURNING:
            break;
    }

    // Кутова швидкість застосовується у будь-якому режимі: у TURNING це поворот на
    // місці, у ACCELERATING/MOVING — корекція курсу під час руху (координований поворот).
    direction_ += currentCmd_.angleSpeed * dt;

    // Інтегруємо позицію вздовж поточного напрямку.
    pos_.x += speed_ * std::cos(direction_) * dt;
    pos_.y += speed_ * std::sin(direction_) * dt;
    timeSecSinceStart_ += dt;

    // Публікуємо телеметрію (коротка критична секція — лише копіювання).
    DroneTelemetry snap{pos_,
                        {speed_ * std::cos(direction_), speed_ * std::sin(direction_)},
                        timeSecSinceStart_,
                        direction_};
    {
        std::lock_guard<std::mutex> lock(mtx_);
        telemetry_ = snap;
    }
}

void DronePhysics::run() {
    ready_.store(true);
    while (!started_.load() && !stop_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    while (!stop_.load()) {
        std::this_thread::sleep_for(std::chrono::duration<float>(dt_ / cfg_.timeScale));
        integrate(dt_);
    }
}

DroneTelemetry DronePhysics::getTelemetry() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return telemetry_;
}
