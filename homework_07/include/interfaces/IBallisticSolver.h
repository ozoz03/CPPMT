#pragma once
#include "Point.h"
#include "AmmoParams.h"

class IBallisticSolver {
    public:
    virtual Point solve(float time, const Point& position, float velocity, const AmmoParams& ammo) = 0; 
    virtual ~IBallisticSolver(){};
};