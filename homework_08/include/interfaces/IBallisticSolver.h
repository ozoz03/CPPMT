#pragma once
#include <memory>
#include "BalisticResult.h"
#include "Target.h"
#include <vector>
#include "MissionContext.h"

struct MissionConfig;
struct AmmoParams;
struct SimStep;
struct Point;


class IBallisticSolver {
    protected:
        float time;
        float distance;
    public:
    std::unique_ptr<IBallisticSolver> createBallisticSolver();

    virtual BalisticResult solve(std::vector<Target>& targets, MissionContext& ctx, const AmmoParams& bomb) = 0; 
    virtual ~IBallisticSolver(){};
};