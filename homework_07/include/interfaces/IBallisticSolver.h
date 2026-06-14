#pragma once
#include <memory>
struct Target;
struct MissionConfig;
struct AmmoParams;
struct SimStep;
struct Point;


class IBallisticSolver {
    public:
    std::unique_ptr<IBallisticSolver> createBallisticSolver();

    virtual Point solve(int currentStepIndex, Target** targets, const MissionConfig& cfg, float currentTime, const AmmoParams& bomb) = 0; 
    virtual int getCurrentTargetIndex() = 0;
    virtual float getCurrentDistance() = 0;
    virtual void setSimSteps(SimStep** steps) = 0;
    virtual ~IBallisticSolver(){};
};