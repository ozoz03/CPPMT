#include "DronePhysics.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

DronePhysics::DronePhysics(const MissionConfig& cfg)
    : cfg_(cfg),
      dt_(cfg.physicsTimeStep),
      // Прискорення з наявних параметрів: v^2 = 2·a·accelPath -> a = attackSpeed^2/(2·accelPath).
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
    DroneCommand cmd;
    while (commands_.tryPop(cmd)) {
        currentCmd_ = cmd;
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
    const float period = dt_ / cfg_.timeScale;
    std::cout << "[physics  " << std::this_thread::get_id()
              << "] thread started, sleeping until start signal" << std::endl;
    ready_.store(true);
    while (!started_.load() && !stop_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::cout << "[physics  " << std::this_thread::get_id() << "] woke on start signal, running"
              << std::endl;

    long ticks = 0;
    while (!stop_.load()) {
        // Sleep for one step, then wake up and integrate motion.
        std::this_thread::sleep_for(std::chrono::duration<float>(period));
        integrate(dt_);
        // Heartbeat: proves the physics thread keeps sleeping/waking each step.
        if (++ticks % 100 == 0) {
            std::cout << "[physics  " << std::this_thread::get_id() << "] tick " << ticks
                      << ": slept " << period << "s -> woke, t=" << timeSecSinceStart_ << "s pos=("
                      << pos_.x << ", " << pos_.y << ") speed=" << speed_ << std::endl;
        }
    }
    std::cout << "[physics  " << std::this_thread::get_id() << "] got stop, exiting after " << ticks
              << " ticks" << std::endl;
}

DroneTelemetry DronePhysics::getTelemetry() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return telemetry_;
}
