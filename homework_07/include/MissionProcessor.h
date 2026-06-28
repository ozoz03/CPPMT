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
#include "Utility.h"


class Mission {
private:
    std::unique_ptr<IBallisticSolver> solver;
    std::unique_ptr<ITargetProvider> targetProvider;    
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
    
    Point computeDrop(MissionContext& ctx) {
        std::cout << "Computing drop for " << targetProvider->getTargets().size() << " targets at step " << ctx.currentStepIndex << std::endl;
        return solver->solve(targetProvider->getTargets(), ctx, currentTime, bomb );
    };

    int getTargetCount() { return targetProvider->getTargetCount(); } ;

    void changeSolver(std::unique_ptr<IBallisticSolver> s) { solver = std::move(s); };

    
    void init(const MissionConfig& cfg, const AmmoParams& bomb) {
        std::cout << "Initializing mission an ammo: " << cfg.ammoName<< std::endl;

        this->simSteps = std::vector<SimStep>(MAX_STEPS);
        SimStep startStep = {cfg.startPos,cfg.initialDir,currentState->name(),-1,0,{0,0},{0,0},{0,0}};
	    // simSteps.push_back(startStep);
        simSteps[0] = startStep;
       
        this->ctx = {0, startStep, cfg, 0,0};
        this->bomb = bomb;

        std::cout << "Mission initialized with MAX_STEPS: " << MAX_STEPS << std::endl;
    }

    bool hasNext() {
        std::cout << "has next" << std::endl;
        // check if current target is hit
		if (solver->getCurrentDistance() <= cfg.hitRadius) {
			std::cout << "Target " << solver->getCurrentTargetIndex() << " is hit!" << std::endl;
			std::cout << "Simulation complete. Steps: " << cycleCount << std::endl;
			return false; // mission complete
		}
        if (this->ctx.currentStepIndex + 1 >= MAX_STEPS) {
            std::cout << "MAX_STEPS reached." << std::endl;
            return false;
        }
        return true; 
    }

    void step()  {
        std::cout << "STEP currentStepIndex: " << this->ctx.currentStepIndex << std::endl;
        // change a context
        if (this->ctx.currentStepIndex > 0) {
            // a new sim step
            SimStep newSimStep = {
                .dronePos = this->ctx.droneContext.dronePos,
                .droneDirection = this->ctx.droneContext.droneDirection,
                .droneStateName = this->currentState->name(),
                .droneSpeed = this->ctx.droneContext.droneSpeed,
                .targetIdx = this->ctx.droneContext.targetIdx,
                .dropPoint = this->ctx.droneContext.dropPoint,
                .aimPoint = Point{},    	
	            .predictedTarget = Point{}
            };
            this->ctx.droneContext = newSimStep;

            auto next = this->currentState->execute(this->ctx); 
            if (next) this->currentState = std::move(next);
		}

        Point dropPoint = computeDrop(this->ctx);
        std::cout << "Computed drop point: (" << dropPoint.x << ", " << dropPoint.y << ")" << std::endl;
        ctx.droneContext.dropPoint = dropPoint;
        
        this->simSteps[this->ctx.currentStepIndex] = ctx.droneContext;

        this->ctx.currentStepIndex++;
        std::cout << "Current Step Index: " << this->ctx.currentStepIndex << std::endl;
    };

    void reset()  { this->ctx.currentStepIndex = 0; };
    void writeDownSteps() {
        writeDownJson(this->simSteps, this->ctx.currentStepIndex);
    }
};