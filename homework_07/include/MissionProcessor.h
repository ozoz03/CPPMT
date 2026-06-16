#pragma once
#include "IBallisticSolver.h"
#include "ITargetProvider.h"
#include "MissionConfig.h"
#include "AmmoParams.h"
#include "Point.h"
#include <iostream>
#include "SimStep.h"

enum DronePhase {STOPPED = 0, ACCELERATING=1, DECELERATING=2, TURNING=3, MOVING=4};

class Mission {
public:
    Mission(IBallisticSolver* solver, ITargetProvider* targetProvider) : solver(solver), targetProvider(targetProvider) {
        this->solver = solver;
        this->targetProvider = targetProvider;
    };
    Point computeDrop(int currentStepIndex, const MissionConfig& cfg) {
        std::cout << "Computing drop for target " << solver->getCurrentTargetIndex() << " at step " << currentStepIndex << std::endl;
        return solver->solve(currentStepIndex, targetProvider->getTargets(), cfg, currentTime, bomb );
    };

    int getTargetCount() { return targetProvider->getTargetCount(); } ;

    void changeSolver(IBallisticSolver* s) { solver = s; };

    
    void init(const MissionConfig& cfg, const AmmoParams& bomb) {
        std::cout << "Initializing mission an ammo: " << cfg .ammoName<< std::endl;
        this->cfg = cfg;
        this->bomb = bomb;
        SimStep** simSteps = new SimStep*[MAX_STEPS];
	    simSteps[0] = new SimStep{ {cfg.startPos.x, cfg.startPos.y}, cfg.initialDir, DronePhase::STOPPED, -1, cfg.startPos, {0,0}, {0,0} };
        solver->setSimSteps(simSteps);
        std::cout << "Mission initialized with config and ammo parameters." << std::endl;
    }

    bool hasNext() {
        cycleCount++;
        // add time step and increment cycle count
		currentTime += cfg.simTimeStep;
        std::cout << "Cycle " << cycleCount << ": Checking if mission has next step..." << std::endl;
        
        // check if current target is hit
		if (solver->getCurrentDistance() <= cfg.hitRadius) {
			std::cout << "Target " << solver->getCurrentTargetIndex() << " is hit!" << std::endl;
			std::cout << "Simulation complete. Steps: " << cycleCount << std::endl;
			return false; // mission complete
		}
        std::cout << "Checking next target " << solver->getCurrentTargetIndex() << ", step " << currentStepIndex << std::endl;
        return currentStepIndex < targetProvider->getTargetCount(); 
    }
    void step()  {
        std::cout << "Processing target " << solver->getCurrentTargetIndex() << std::endl;
        // Simulate the step and calculate distance to target
        std::cout << "Calculating drop for target " << solver->getCurrentTargetIndex() << " at step " << currentStepIndex << std::endl;
        currentStepIndex++;
        Point dropPoint = computeDrop(currentStepIndex, cfg);
        std::cout << "Computed drop point: (" << dropPoint.x << ", " << dropPoint.y << ")" << std::endl;
    };
    void reset()  { currentStepIndex = 0; };
private:
    IBallisticSolver* solver;
    ITargetProvider*  targetProvider;    
    int currentStepIndex = 0;
    MissionConfig cfg;
    AmmoParams bomb;
    int cycleCount = 0;
    float currentTime = 0.0f;
    const int MAX_STEPS = 1000;  
};