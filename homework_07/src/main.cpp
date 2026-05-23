#include "AnalyticalSolver.cpp"
#include "TableSolver.hpp"
#include "Mission.hpp"
#include "JsonTargetProvider.cpp"



int main() {

    JsonTargetProvider provider("targets.json");
    AnalyticalSolver   analytical;
    TableSolver        table(&ballisticData);

    Mission mission;
    mission.init(configSource);
    mission.changeSolver(analytical);

    while (mission.hasNext()) {
        mission.step(); 
    };
    
    return 0;
}
