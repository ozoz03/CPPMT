#include "AnalyticalSolver.h"
#include "BalisticResult.h"
#include "MissionContext.h"
#include "Target.h"
#include "Utility.h"

	
BalisticResult AnalyticalSolver::solve(std::vector<Target>& targets, MissionContext& ctx, const AmmoParams& bomb) {
        this->time = getTimeByCardano(bomb, ctx.cfg);
	std::cout << "Time of Flight: " << this->time << std::endl;

	this->distance = getDistanceByTime(time, bomb, ctx.cfg);
	std::cout << "h distance: " << this->distance << std::endl;
	return getBalisticResult(this->time, this->distance, targets, ctx.droneContext, ctx.cfg);		
};

