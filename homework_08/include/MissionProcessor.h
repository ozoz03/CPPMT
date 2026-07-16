#pragma once
#include "MissionContext.h"
#include "IBallisticSolver.h"
#include "ITargetProvider.h"
#include "DronePhysics.h"
#include "MissionConfig.h"
#include "AmmoParams.h"
#include "SimStep.h"
#include "IDroneState.h"
#include "Utility.h"
#include <atomic>
#include <memory>
#include <vector>

// ДЗ18, потік 3. Логіка місії: вибір цілі, балістика, стейт-машина.
// Не володіє і не інтегрує стан дрона — щоразу запитує телеметрію у фізики
// й керує нею командами через чергу. Це головний потік симуляції: коли він
// завершується (ціль вражено / MAX_STEPS), main зупиняє решту.
class MissionProcessor {
public:
    MissionProcessor(std::unique_ptr<IBallisticSolver> solver,
                     ITargetProvider* provider,
                     DronePhysics* physics,
                     std::unique_ptr<IDroneState> initialState)
        : solver(std::move(solver)), provider(provider), physics(physics),
          currentState(std::move(initialState)) {}

    void init(const MissionConfig& cfg, const AmmoParams& bomb);

    void run();
    bool isThreadReady() const { return ready_.load(); }
    void start() { started_.store(true); }
    void stop() { stop_.store(true); }

    void writeDownSteps() { writeDownJson(this->simSteps, this->ctx.currentStepIndex); }
    MissionContext& getMissionContext() { return this->ctx; }

private:
    void planStep();  // один крок планування

    std::unique_ptr<IBallisticSolver> solver;
    ITargetProvider* provider;   // не володіє (лише знімки)
    DronePhysics* physics;        // не володіє (телеметрія + команди)
    AmmoParams bomb;
    MissionContext ctx;
    std::unique_ptr<IDroneState> currentState;
    std::vector<SimStep> simSteps;
    int currentTargetIdx_ = -1;   // поточна обрана ціль (персистентно між кроками)
    const int MAX_STEPS = 10000;

    std::atomic<bool> ready_{false};
    std::atomic<bool> started_{false};
    std::atomic<bool> stop_{false};
};
