#include "IBallisticSolver.h"
#include "Point.h"
#include "MissionConfig.h"
#include "Target.h"
#include "SimStep.h"
#include <array>
#include <iostream>

class AnalyticalSolver : public IBallisticSolver {
    private:
        SimStep** simSteps;
        int currentTargetIndex = 0;
		std::array<float, 5> targetDistances;
		double targetsToDroneAngleRadians[5];
		double targetAngleDiff[5];

    public:
    AnalyticalSolver(){
		std::cout << "Created a Analytical solver" << std::endl;
	}

	void setSimSteps(SimStep** steps) override{
		this->simSteps = steps;
	}
	
	int getCurrentTargetIndex() override {
		return currentTargetIndex;
	}

	float getCurrentDistance() override {
		std::cout << "Current distance to target " << currentTargetIndex << " is " << targetDistances[currentTargetIndex] << std::endl;
		return targetDistances[currentTargetIndex];
	}
	
    Point solve(int currentStepIndex, Target** targets, const MissionConfig& cfg, float currentTime, const AmmoParams& bomb) override;
    ~AnalyticalSolver() override {};   
};  