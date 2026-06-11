#include "AnalyticalSolver.cpp"
#include "Mission.cpp"
#include "JsonTargetProvider.cpp"
#include "JsonConfigLoader.cpp"
#include "MissionConfig.h"
#include "AmmoParams.h" 
#include "config.hpp"


int main() {
    JsonConfigLoader  jsonConfigLoader;
    jsonConfigLoader.load(DATA_DIR_PATH.data() + std::string("/config.json"));
    MissionConfig missionConfig = jsonConfigLoader.getConfig();
    AmmoParams bomb = jsonConfigLoader.getAmmoParams();

    JsonTargetProvider provider(DATA_DIR_PATH.data() + std::string("/targets.json"));
    AnalyticalSolver analytical;
    Mission mission(&analytical, &provider);
    mission.init(missionConfig, bomb);

    do {
        mission.step();    
    } while (mission.hasNext());
    
    return 0;
}
