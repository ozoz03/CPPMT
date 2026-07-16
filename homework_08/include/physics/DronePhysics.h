#pragma once
#include "DroneCommand.h"
#include "DroneTelemetry.h"
#include "MissionConfig.h"
#include "Point.h"
#include "ThreadSafeQueue.h"
#include <atomic>
#include <mutex>

// ДЗ18, потік 2. Володіє станом дрона (позиція, швидкість, напрямок).
// Приймає команди через ThreadSafeQueue і кожні physicsTimeStep інтегрує рух.
// MissionProcessor власної копії стану не тримає — лише запитує getTelemetry().
class DronePhysics {
public:
    explicit DronePhysics(const MissionConfig& cfg);

    // Черга команд (продюсер — MissionProcessor).
    void sendCommand(const DroneCommand& cmd) { commands_.push(cmd); }

    // Копія телеметрії під м'ютексом.
    DroneTelemetry getTelemetry() const;

    void run();
    bool isThreadReady() const { return ready_.load(); }
    void start() { started_.store(true); }
    void stop() { stop_.store(true); }

private:
    void integrate(float dt);  // один крок інтегрування за поточною командою

    MissionConfig cfg_;
    float dt_;         // physicsTimeStep
    float accel_;      // прискорення/гальмування, м/с²

    ThreadSafeQueue<DroneCommand> commands_;
    DroneCommand currentCmd_;  // остання застосована команда

    // Внутрішній стан (володіє фізика):
    Coord pos_;
    float direction_;
    float speed_;               // скалярна швидкість
    float timeSecSinceStart_;   // час останнього оновлення фізики

    mutable std::mutex mtx_;    // захищає телеметрію
    DroneTelemetry telemetry_;  // опублікований знімок

    std::atomic<bool> ready_{false};
    std::atomic<bool> started_{false};
    std::atomic<bool> stop_{false};
};
