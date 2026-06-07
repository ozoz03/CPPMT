#pragma once
#include "Point.h"
#include "MissionConfig.h"
#include "Target.h"
#include "AmmoParams.h"
#include "SimStep.h"

class IBallisticSolver {
    public:
    virtual Point solve(int currentStepIndex, Target** targets, const MissionConfig& cfg, float currentTime, const AmmoParams& bomb) = 0; 
    virtual int getCurrentTargetIndex() = 0;
    virtual float getCurrentDistance() = 0;
    virtual void setSimSteps(SimStep** steps) = 0;
    virtual ~IBallisticSolver(){};
};