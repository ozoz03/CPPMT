#include "IBallisticSolver.h"

class AnalyticalSolver : public IBallisticSolver {
public:
    init(){};
    DropPoint computeDrop(float time, const Position& position, float velocity, const AmmoParams& ammo) override;
    
    ~AnalyticalSolver() override {};   
};  