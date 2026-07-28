#include "TableSolver.h"
#include "BalisticResult.h"
#include "MissionContext.h"
#include "Target.h"
#include "Utility.h"
#include <iostream>

BalisticResult TableSolver::solve(std::vector<Target>& targets, MissionContext& ctx, const AmmoParams& bomb) {
    // Використовуємо таблицю для отримання результатів
    auto result = table_.lookup(ctx.cfg.altitude, ctx.droneContext.droneSpeed, bomb.mass, bomb.drag, bomb.lift);
    return getBalisticResult(result.t, result.hDist, targets, ctx.droneContext, ctx.cfg);
}