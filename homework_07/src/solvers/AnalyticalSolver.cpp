#include "AnalyticalSolver.h"
#include "Point.h"
#include "MissionConfig.h"
#include "Target.h"
#include "DronePhase.h"
#include <iostream>
#include <ostream>
#include "Utility.h"


	
Point AnalyticalSolver::solve(int currentStepIndex, std::vector<Target>& targets, const MissionConfig& cfg, float currentTime, const AmmoParams& bomb) {
        
        if (currentStepIndex > 0) {
			simSteps[currentStepIndex] = simSteps[currentStepIndex-1];
		} else {
            simSteps[currentStepIndex] = { {cfg.startPos.x, cfg.startPos.y}, cfg.initialDir, STOPPED, -1, cfg.startPos, {0,0}, {0,0} };
        }

		std::cout << "Current time: " << currentTime << std::endl;

		// update drone phase
		switch (simSteps[currentStepIndex].state) {
		case STOPPED:
			if (currentTime > 0) {
				simSteps[currentStepIndex].state = ACCELERATING;
			}
			break;
		case ACCELERATING:
			if (currentTime > cfg.accelPath / cfg.attackSpeed) {
				simSteps[currentStepIndex].state = MOVING;
			}
			break;
		case DECELERATING: {
			simSteps[currentStepIndex].state = TURNING;
			break;
		}
		}

        this->targetsToDroneAngleRadians.resize(targets.size());
		this->targetsAngleDiff.resize(targets.size());
		targetDistances = calculateTargetDistances(currentTime, targets, simSteps[currentStepIndex], cfg, this->targetsToDroneAngleRadians);

		std::vector<float> targetDistanceTimes = getFlightTimeToTarget(targetDistances, cfg);

		// add turn time to flight time
		for (std::size_t i=0; i < targetsToDroneAngleRadians.size(); ++i) {
			targetDistanceTimes[i] += getTurnTime(i, simSteps[currentStepIndex], targetsToDroneAngleRadians[i], targetsAngleDiff, cfg);
			std::cout << "Total time to target [" << i << "] = " << targetDistanceTimes[i] << std::endl;
		}
		// get the nearest target
		int nearestTargetIndex = getIndexOfMin(targetDistanceTimes);
		std::cout << "The nearest target is " << nearestTargetIndex << std::endl;
		simSteps[currentStepIndex].targetIdx = nearestTargetIndex;

		if (this->currentTargetIndex != nearestTargetIndex) {
			this->currentTargetIndex = nearestTargetIndex;
			std::cout << "Switching to target " << currentTargetIndex << std::endl;
			if (simSteps[currentStepIndex].state == MOVING) {
				simSteps[currentStepIndex].state = DECELERATING;
				std::cout << "Decelerating to switch target" << std::endl;
			}
		}

	
		if (simSteps[currentStepIndex].state == TURNING) {
			doTurn(targetsAngleDiff[currentTargetIndex], simSteps[currentStepIndex], currentTargetIndex, cfg);
			if (std::abs(simSteps[currentStepIndex].direction) < cfg.turnThreshold) {
				simSteps[currentStepIndex].state = MOVING;
			}
		} else {
			// update drone position
			doMove(simSteps[currentStepIndex], cfg);
		}

		calculateBalistics(bomb, targets, simSteps[currentStepIndex], cfg);
		
		// output
        std::stringstream s1, s2, s3, s4, s5;

		std::cout << "currentStepIndex" << currentStepIndex << std::endl;
		s1.str("");
		s1 << currentStepIndex;
		s2 << " (" << simSteps[currentStepIndex].pos.x << ", " << simSteps[currentStepIndex].pos.y << ", " << cfg.altitude << ")";
		s3  << simSteps[currentStepIndex].direction << " ";
		s4  << statusToString(simSteps[currentStepIndex].state) << " ";
		s5  << currentTargetIndex << " ";		
		writeStringIntoFile(s1, s2, s3, s4, s5);
		
		std::cout << "Step " << currentStepIndex << " pos=(" << simSteps[currentStepIndex].pos.x << "," << simSteps[currentStepIndex].pos.y << ")" << std::endl;
		std::cout << "  target=" << currentTargetIndex << " state=" << statusToString(simSteps[currentStepIndex].state) << std::endl;

        return simSteps[currentStepIndex].dropPoint;
};
