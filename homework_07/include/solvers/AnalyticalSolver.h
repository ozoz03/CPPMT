#include "IBallisticSolver.h"
#include "Point.h"
#include "MissionConfig.h"
#include "Target.h"
#include "SimStep.h"
#include <iostream>
#include <vector>

class AnalyticalSolver : public IBallisticSolver {
    private:
        std::vector<SimStep> simSteps;
        int currentTargetIndex = 0;
		std::vector<float> targetDistances;
		std::vector<double> targetsToDroneAngleRadians;
		std::vector<double> targetsAngleDiff;

    public:
    AnalyticalSolver(){
		std::cout << "Created a Analytical solver" << std::endl;
	}

	void setSimSteps(std::vector<SimStep> steps) override{
		this->simSteps = steps;
	}
	
	int getCurrentTargetIndex() override {
		return currentTargetIndex;
	}

	float getCurrentDistance() override {
		std::cout << "Current distance to target " << currentTargetIndex << " is " << targetDistances[currentTargetIndex] << std::endl;
		return targetDistances[currentTargetIndex];
	}
	
    Point solve(int currentStepIndex, std::vector<Target>& targets, const MissionConfig& cfg, float currentTime, const AmmoParams& bomb) override;
    ~AnalyticalSolver() override {};   
};  