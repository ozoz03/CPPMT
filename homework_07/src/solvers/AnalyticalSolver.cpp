#include "AnalyticalSolver.h"
#include "BalisticResult.h"
#include "MissionContext.h"
#include "Target.h"
#include "Utility.h"

	
BalisticResult AnalyticalSolver::solve(std::vector<Target>& targets, MissionContext& ctx, const AmmoParams& bomb) {
			
        return calculateBalistics(bomb, targets, ctx.droneContext, ctx.cfg);
};
