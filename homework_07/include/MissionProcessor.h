#pragma once
#include "BalisticResult.h"
#include "MissionContext.h"
#include "IBallisticSolver.h"
#include "ITargetProvider.h"
#include "MissionConfig.h"
#include "AmmoParams.h"
#include "Point.h"
#include <iostream>
#include "SimStep.h"
#include "IDroneState.h"
#include "StateStopped.h"
#include "Utility.h"


class MissionProcessor {
private:
    std::unique_ptr<IBallisticSolver> solver;
    std::unique_ptr<ITargetProvider> targetProvider;    
    AmmoParams bomb;
    MissionContext ctx;
    std::unique_ptr<IDroneState> currentState;
    std::vector<SimStep> simSteps;
    const int MAX_STEPS = 10000;      
public:
    MissionProcessor(std::unique_ptr<IBallisticSolver> solver, std::unique_ptr<ITargetProvider> targetProvider, std::unique_ptr<IDroneState> currentState) : 
        solver(std::move(solver)), targetProvider(std::move(targetProvider)), currentState(std::move(currentState)) {};
    
    Point computeDrop(MissionContext& ctx);

    int getTargetCount() { return targetProvider->getTargetCount(); } ;

    void changeSolver(std::unique_ptr<IBallisticSolver> s) { solver = std::move(s); };

    
    void init(const MissionConfig& cfg, const AmmoParams& bomb);

    bool hasNext();

    void step();

    void reset()  { this->ctx.currentStepIndex = 0; };
    void writeDownSteps() {
        writeDownJson(this->simSteps, this->ctx.currentStepIndex);
    }
    MissionContext& getMissionContext() { return this->ctx; };
};