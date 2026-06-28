#include "AnalyticalSolver.h"
#include "Point.h"
#include "MissionContext.h"
#include "Target.h"
#include <iostream>
#include <ostream>
#include "Utility.h"

	
Point AnalyticalSolver::solve(std::vector<Target>& targets, MissionContext& ctx, float currentTime, const AmmoParams& bomb) {
		std::cout << "Current time: " << currentTime << std::endl;

        this->targetsToDroneAngleRadians.resize(targets.size());
		this->targetsAngleDiff.resize(targets.size());
		targetDistances = calculateTargetDistances(currentTime, targets, ctx.droneContext, ctx.cfg, this->targetsToDroneAngleRadians);

		std::vector<float> targetDistanceTimes = getFlightTimeToTarget(targetDistances, ctx.cfg);

		// add turn time to flight time
		for (std::size_t i=0; i < targetsToDroneAngleRadians.size(); ++i) {
			// targetDistanceTimes[i] += getTurnTime(i, ctx.droneContext, targetsToDroneAngleRadians[i], targetsAngleDiff, ctx.cfg);
			targetDistanceTimes[i] += (ctx.turnRemaining * ctx.cfg.simTimeStep);
			std::cout << "Total time to target [" << i << "] = " << targetDistanceTimes[i] << std::endl;
		}
		// get the nearest target
		int nearestTargetIndex = getIndexOfMin(targetDistanceTimes);
		std::cout << "The nearest target is " << nearestTargetIndex << std::endl;
		

		if (this->currentTargetIndex != nearestTargetIndex) {
			this->currentTargetIndex = nearestTargetIndex;
			std::cout << "Switching to target " << currentTargetIndex << std::endl;
			// change the context
			ctx.droneContext.targetIdx = nearestTargetIndex;
			ctx.desiredDir = this->targetsToDroneAngleRadians[nearestTargetIndex];
		}

		calculateBalistics(bomb, targets, ctx.droneContext, ctx.cfg);
		
		// output
        std::stringstream s1, s2, s3, s4, s5;

		s1.str("");
		s1 << ctx.currentStepIndex;
		s2 << " (" << ctx.droneContext.dronePos.x << ", " << ctx.droneContext.dronePos.y << ", " << ctx.cfg.altitude << ")";
		s3  << ctx.droneContext.droneDirection << " ";
		s4  << ctx.droneContext.droneStateName << " ";
		s5  << currentTargetIndex << " ";		
		writeStringIntoFile(s1, s2, s3, s4, s5);
		
		std::cout << "Step " << ctx.currentStepIndex << " pos=(" << ctx.droneContext.dronePos.x << "," << ctx.droneContext.dronePos.y << ")" << std::endl;
		std::cout << "  target=" << currentTargetIndex << " state=" << ctx.droneContext.droneStateName << std::endl;

        return ctx.droneContext.dropPoint;
};
