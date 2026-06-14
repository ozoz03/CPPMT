#include "AnalyticalSolver.h"
#include "Point.h"
#include "MissionConfig.h"
#include "Target.h"
#include "SimStep.h"
#include "DronePhase.h"
#include <array>
#include <iostream>
#include "Utility.h"


	
Point AnalyticalSolver::solve(int currentStepIndex, Target** targets, const MissionConfig& cfg, float currentTime, const AmmoParams& bomb) {
        
        if (currentStepIndex > 0) {
			    simSteps[currentStepIndex] = new SimStep(*simSteps[currentStepIndex-1]);
		} else {
            simSteps[currentStepIndex] = new SimStep{ {cfg.startPos.x, cfg.startPos.y}, cfg.initialDir, STOPPED, -1, cfg.startPos, {0,0}, {0,0} };
        }

		std::cout << "Current time: " << currentTime << std::endl;

		// update drone phase
		switch (simSteps[currentStepIndex]->state) {
		case STOPPED:
			if (currentTime > 0) {
				simSteps[currentStepIndex]->state = ACCELERATING;
			}
			break;
		case ACCELERATING:
			if (currentTime > cfg.accelPath / cfg.attackSpeed) {
				simSteps[currentStepIndex]->state = MOVING;
			}
			break;
		case DECELERATING: {
			simSteps[currentStepIndex]->state = TURNING;
			break;
		}
		}

        double targetToDroneAngleRadians[5]= {0,0,0,0,0};
		targetDistances = calculateTargetDistances(currentTime, targets, simSteps[currentStepIndex], cfg, targetsToDroneAngleRadians);

		std::array<float, 5> targetDistanceTimes = getFlightTimeToTarget(targetDistances, cfg);

		// add turn time to flight time
		for (int i=0; i < 5; ++i) {
			targetDistanceTimes[i] += getTurnTime(i, simSteps[currentStepIndex], targetToDroneAngleRadians[i], targetAngleDiff, cfg);
			std::cout << "Total time to target [" << i << "] = " << targetDistanceTimes[i] << std::endl;
		}
		// get the nearest target
		int nearestTargetIndex = getIndexOfMin(targetDistanceTimes);
		std::cout << "The nearest target is " << nearestTargetIndex << std::endl;
		simSteps[currentStepIndex]->targetIdx = nearestTargetIndex;

		if (currentTargetIndex != nearestTargetIndex) {
			currentTargetIndex = nearestTargetIndex;
			std::cout << "Switching to target " << currentTargetIndex << std::endl;
			if (simSteps[currentStepIndex]->state == MOVING) {
				simSteps[currentStepIndex]->state = DECELERATING;
				std::cout << "Decelerating to switch target" << std::endl;
			}
		}

	
		if (simSteps[currentStepIndex]->state == TURNING) {
			doTurn(targetAngleDiff[currentTargetIndex], simSteps[currentStepIndex], currentTargetIndex, cfg);
			if (std::abs(simSteps[currentStepIndex]->direction) < cfg.turnThreshold) {
				simSteps[currentStepIndex]->state = MOVING;
			}
		} else {
			// update drone position
			doMove(simSteps[currentStepIndex], cfg);
		}

		calculateBalistics(bomb, targets, simSteps[currentStepIndex], cfg);
		
		// output
        std::stringstream s1, s2, s3, s4, s5;

		s1.str("");
		s1 << currentStepIndex;
		s2 << " (" << simSteps[currentStepIndex]->pos.x << ", " << simSteps[currentStepIndex]->pos.y << ", " << cfg.altitude << ")";
		s3  << simSteps[currentStepIndex]->direction << " ";
		s4  << statusToString(simSteps[currentStepIndex]->state) << " ";
		s5  << currentTargetIndex << " ";		
		writeStringIntoFile(s1, s2, s3, s4, s5);
		
		std::cout << "Step " << currentStepIndex << " pos=(" << simSteps[currentStepIndex]->pos.x << "," << simSteps[currentStepIndex]->pos.y << ")" << std::endl;
		std::cout << "  target=" << currentTargetIndex << " state=" << statusToString(simSteps[currentStepIndex]->state) << std::endl;

        return simSteps[currentStepIndex]->dropPoint;
};
