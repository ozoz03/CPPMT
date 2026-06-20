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
private:
    std::unique_ptr<IBallisticSolver> solver;
    std::unique_ptr<ITargetProvider> targetProvider;    
    int currentStepIndex = 0;
    MissionConfig cfg;
    AmmoParams bomb;
    int cycleCount = 0;
    float currentTime = 0.0f;
    const int MAX_STEPS = 200;      
public:
    Mission(std::unique_ptr<IBallisticSolver> solver, std::unique_ptr<ITargetProvider> targetProvider) : solver(std::move(solver)), targetProvider(std::move(targetProvider)) {};
    
    Point computeDrop(int currentStepIndex, const MissionConfig& cfg) {
        std::cout << "Computing drop for " << targetProvider->getTargets().size() << " targets at step " << currentStepIndex << std::endl;
        return solver->solve(currentStepIndex, targetProvider->getTargets(), cfg, currentTime, bomb );
    };

    int getTargetCount() { return targetProvider->getTargetCount(); } ;

    void changeSolver(std::unique_ptr<IBallisticSolver> s) { solver = std::move(s); };

    
    void init(const MissionConfig& cfg, const AmmoParams& bomb) {
        std::cout << "Initializing mission an ammo: " << cfg.ammoName<< std::endl;
        this->cfg = cfg;
        this->bomb = bomb;
        std::vector<SimStep> simSteps(MAX_STEPS);
	    simSteps[0] = { {cfg.startPos.x, cfg.startPos.y}, cfg.initialDir, DronePhase::STOPPED, -1, cfg.startPos, {0,0}, {0,0} };
        solver->setSimSteps(simSteps);
        std::cout << "Mission initialized with MAX_STEPS: " << MAX_STEPS << std::endl;

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
        if (currentStepIndex >= MAX_STEPS) {
            std::cout << "MAX_STEPS reached." << std::endl;
            return false;
        }
        return true; 
    }

    void step()  {
        currentStepIndex++;
        Point dropPoint = computeDrop(currentStepIndex, cfg);
        std::cout << "Computed drop point: (" << dropPoint.x << ", " << dropPoint.y << ")" << std::endl;
    };

    void reset()  { currentStepIndex = 0; };
};