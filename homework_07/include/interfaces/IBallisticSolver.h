#pragma once

class IBallisticSolver {
    public:
    virtual solve(dronePos, targetPos, altitude, ammo...) = 0; 
    virtual ~IBallisticSolver(){};
}