#include "AnalyticalSolver.cpp"
#include "Mission.cpp"
#include "JsonTargetProvider.cpp"
#include "JsonConfigLoader.cpp"
#include "MissionConfig.h"
#include "IBallisticSolver.h"
#include "IMission.h"
#include "ITargetProvider.h"
#include "AmmoParams.h" 



int main() {
    JsonConfigLoader  jsonConfigLoader;
    jsonConfigLoader.load("config.json");
    MissionConfig configSource = jsonConfigLoader.getConfig();

    JsonTargetProvider provider("targets.json");
    AnalyticalSolver   analytical;
    Mission mission(&analytical, &provider);
    mission.init(configSource);

    while (mission.hasNext()) {
        mission.step();    
    };
    
    return 0;
}
