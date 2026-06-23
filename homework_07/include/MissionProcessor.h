#pragma once
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

// enum DronePhase {STOPPED = 0, ACCELERATING=1, DECELERATING=2, TURNING=3, MOVING=4};

class Mission {
private:
    std::unique_ptr<IBallisticSolver> solver;
    std::unique_ptr<ITargetProvider> targetProvider;    
    int currentStepIndex = 0;
    MissionConfig cfg;
    AmmoParams bomb;
    MissionContext ctx;
    std::unique_ptr<IDroneState> currentState;
    std::vector<SimStep> simSteps;
    int cycleCount = 0;
    float currentTime = 0.0f;
    const int MAX_STEPS = 10000;      
public:
    Mission(std::unique_ptr<IBallisticSolver> solver, std::unique_ptr<ITargetProvider> targetProvider, std::unique_ptr<IDroneState> currentState) : 
        solver(std::move(solver)), targetProvider(std::move(targetProvider)), currentState(std::move(currentState)) {};
    
    Point computeDrop(int currentStepIndex, MissionContext& ctx) {
        std::cout << "Computing drop for " << targetProvider->getTargets().size() << " targets at step " << currentStepIndex << std::endl;
        return solver->solve(currentStepIndex, targetProvider->getTargets(), ctx, currentTime, bomb );
    };

    int getTargetCount() { return targetProvider->getTargetCount(); } ;

    void changeSolver(std::unique_ptr<IBallisticSolver> s) { solver = std::move(s); };

    
    void init(const MissionConfig& cfg, const AmmoParams& bomb) {
        std::cout << "Initializing mission an ammo: " << cfg.ammoName<< std::endl;

        simSteps = std::vector<SimStep>(MAX_STEPS);
        SimStep startStep = {cfg.startPos,cfg.initialDir,"Stopped",-1,0,{0,0},{0,0},{0,0}};
	    simSteps.push_back(startStep);
       
        this->ctx = {simSteps[0], cfg, 0,0,0};
        this->bomb = bomb;

        std::cout << "Mission initialized with MAX_STEPS: " << MAX_STEPS << std::endl;
    }

    bool hasNext() {
        // check if current target is hit
		if (solver->getCurrentDistance() <= cfg.hitRadius) {
			std::cout << "Target " << solver->getCurrentTargetIndex() << " is hit!" << std::endl;
			std::cout << "Simulation complete. Steps: " << cycleCount << std::endl;
			return false; // mission complete
		}
        if (currentStepIndex + 1 >= MAX_STEPS) {
            std::cout << "MAX_STEPS reached." << std::endl;
            return false;
        }
        return true; 
    }

    void step()  {

        // change a context
        if (currentStepIndex > 0) {
            // a new sim step
            SimStep newSimStep = simSteps.back();

            auto next = this->currentState->execute(ctx); 
            if (next) this->currentState = std::move(next);

            newSimStep.droneStateName = this->currentState->name();

            ctx.droneContext = newSimStep;
		}

        Point dropPoint = computeDrop(currentStepIndex, ctx);
        std::cout << "Computed drop point: (" << dropPoint.x << ", " << dropPoint.y << ")" << std::endl;
        ctx.droneContext.dropPoint = dropPoint;
        
        simSteps.push_back(ctx.droneContext);

        currentStepIndex++;
        std::cout << "Current Step Index: " << currentStepIndex << std::endl;
    };

    void reset()  { currentStepIndex = 0; };
};