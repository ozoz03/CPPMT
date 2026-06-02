#include "IMission.h"
#include "IBallisticSolver.h"
#include "ITargetProvider.h"
#include "Target.h"
#include "MissionConfig.h"
#include "AmmoParams.h"
#include "MissionBuilder.cpp"
#include "Point.h"
#include <iostream>


class Mission : public IMission {
    IBallisticSolver* solver;   // стратегія
    ITargetProvider*  targets;
private:
    int currentTargetIndex; 
    std::vector<float> targetDistances;
    MissionConfig cfg;
    int cycleCount = 0;

public:
    Mission(IBallisticSolver* s, ITargetProvider* t) : solver(s), targets(t) {}
 
    Point computeDrop(int currentStepIndex, Point dronePos, int currentTargetIndex, const AmmoParams& ammo) override{
        Target t = targets->getTarget(currentTargetIndex);
        return solver->solve(0.1f, dronePos, t.positions[currentStepIndex], ammo);
    };

    int getTargetCount() override { return targets->getTargetCount(); } ;

    void changeSolver(const IBallisticSolver& s) override { solver = &s; };

    
    void init(const MissionConfig& cfg) override {
        this->cfg = cfg;
    };

    bool hasNext() override{
        // check if current target is hit
		if (targetDistances[currentTargetIndex] <= cfg.hitRadius) {
			std::cout << "Target " << currentTargetIndex << " is hit!" << std::endl;
			LOG("Simulation complete. Steps: " << cycleCount);
			return false; // mission complete
		} 
        return currentTargetIndex < targets->getTargetCount(); 
    }
    void step() override { currentTargetIndex++; };
    void reset() override { currentTargetIndex = 0; };
};