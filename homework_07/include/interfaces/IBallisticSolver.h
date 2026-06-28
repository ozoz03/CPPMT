#pragma once
#include <memory>
#include "Target.h"
#include <vector>
#include "MissionContext.h"

struct MissionConfig;
struct AmmoParams;
struct SimStep;
struct Point;


class IBallisticSolver {
    public:
    std::unique_ptr<IBallisticSolver> createBallisticSolver();

    virtual Point solve(std::vector<Target>& targets, MissionContext& ctx, float currentTime, const AmmoParams& bomb) = 0; 
    virtual int getCurrentTargetIndex() = 0;
    virtual float getCurrentDistance() = 0;
    virtual ~IBallisticSolver(){};
};