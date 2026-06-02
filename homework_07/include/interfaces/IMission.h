#pragma once
#include "IBallisticSolver.h"
#include "MissionConfig.h"
#include "AmmoParams.h"
#include "Point.h"

class IMission {
    public:
    virtual void init(const MissionConfig& configSource) = 0;
    virtual bool hasNext() = 0;
    virtual void step() = 0;
    virtual void reset() = 0;
    virtual void changeSolver(const IBallisticSolver& s) = 0;
    virtual Point computeDrop(int currentStepIndex, Point dronePos, int currentTargetIndex, const AmmoParams& ammo) = 0;
    virtual int getTargetCount() = 0;

    virtual ~IMission(){};
};