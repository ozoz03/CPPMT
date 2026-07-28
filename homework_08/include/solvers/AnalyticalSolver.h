#include "BalisticResult.h"
#include "IBallisticSolver.h"
#include "Target.h"
#include <iostream>
#include <vector>

class AnalyticalSolver : public IBallisticSolver {

    public:
    AnalyticalSolver(){
		std::cout << "Created a Analytical solver" << std::endl;
	}

    BalisticResult solve(std::vector<Target>& targets, MissionContext& ctx, const AmmoParams& bomb) override;
    ~AnalyticalSolver() override {};   
};  