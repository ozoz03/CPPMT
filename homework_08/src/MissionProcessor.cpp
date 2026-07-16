#include "MissionProcessor.h"
#include "TargetDistance.h"
#include "DroneTelemetry.h"
#include "DronePhase.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

void MissionProcessor::init(const MissionConfig& cfg, const AmmoParams& bomb) {
    this->bomb = bomb;
    this->simSteps = std::vector<SimStep>(MAX_STEPS);

    SimStep startStep{};
    startStep.dronePos = cfg.startPos;
    startStep.droneDirection = cfg.initialDir;
    startStep.droneStateName = currentState->name();
    startStep.targetIdx = -1;

    this->ctx = MissionContext{};
    this->ctx.currentStepIndex = 0;
    this->ctx.droneContext = startStep;
    this->ctx.cfg = cfg;
    this->ctx.desiredDir = cfg.initialDir;
    this->ctx.turnRemaining = 0.0f;
    this->ctx.command = {STOPPED, 0.0f};
}

void MissionProcessor::planStep() {
    // 1. Запит телеметрії у фізики (копія, без власної копії стану дрона).
    DroneTelemetry tel = physics->getTelemetry();
    ctx.droneContext.dronePos = tel.pos;
    ctx.droneContext.droneDirection = tel.direction;
    ctx.droneContext.droneSpeed = std::sqrt(tel.speed.x * tel.speed.x + tel.speed.y * tel.speed.y);
    ctx.droneContext.currentTime = tel.timeSecSinceStart;
    ctx.droneContext.targetIdx = currentTargetIdx_;

    // 2. Знімок цілей від провайдера.
    std::vector<Target> targets = provider->getTargets();
    if (targets.empty()) {
        return;
    }

    // 3. Вибір цілі.
    TargetDistance nearest = getNearestTarget(ctx, targets);
    if (currentTargetIdx_ != nearest.targetIndex) {
        currentTargetIdx_ = nearest.targetIndex;
        ctx.droneContext.targetIdx = nearest.targetIndex;
        ctx.desiredDir = nearest.angleToDroneRadians;
    }

    // 4. Стейт-машина → команда фізиці.
    auto next = currentState->execute(ctx);
    if (next) currentState = std::move(next);
    physics->sendCommand(ctx.command);

    // 5. Балістика по обраній цілі.
    BalisticResult result = solver->solve(targets, ctx, bomb);
    ctx.droneContext.dropPoint = result.dropPoint;
    ctx.droneContext.aimPoint = result.aimPoint;
    ctx.droneContext.predictedTarget = result.predictedTarget;
    ctx.droneContext.droneStateName = currentState->name();

    // 6. Лог кроку планування.
    if (ctx.currentStepIndex < MAX_STEPS) {
        simSteps[ctx.currentStepIndex] = ctx.droneContext;
        ctx.currentStepIndex++;
    }

    // 7. Умови завершення (перевіряє потік місії — він головний).
    if (ctx.droneContext.targetDistance <= ctx.cfg.hitRadius) {
        std::cout << "Target " << ctx.droneContext.targetIdx << " is hit! Steps: "
                  << ctx.currentStepIndex << std::endl;
        stop_.store(true);
        return;
    }
    if (ctx.currentStepIndex + 1 >= MAX_STEPS) {
        std::cout << "MAX_STEPS reached." << std::endl;
        stop_.store(true);
    }
}

void MissionProcessor::run() {
    ready_.store(true);
    while (!started_.load() && !stop_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    while (!stop_.load()) {
        planStep();
        std::this_thread::sleep_for(
            std::chrono::duration<float>(ctx.cfg.simTimeStep / ctx.cfg.timeScale));
    }

    // Лог веде MissionProcessor.
    writeDownSteps();
    writeOutputFile(ctx);
}
