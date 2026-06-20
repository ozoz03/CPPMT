#include "MissionProcessor.h"
#include "config.hpp"
#include "ComponentFactory.h"
#include "IConfigLoader.h"
#include "SourceType.h"
#include <iostream>

int main() {
    std::string fileName = DATA_DIR_PATH.data() + std::string("/config.json");
    std::cout << "Loading config from: " << fileName << std::endl;
    auto jsonConfigLoader = ConfigLoaderFactory::createConfigLoader(fileName);
    MissionConfig missionConfig = jsonConfigLoader->getConfig();
    AmmoParams bomb = jsonConfigLoader->getAmmoParams();

    std::string filePath = DATA_DIR_PATH.data() + std::string("/targets.json");
    Mission mission = Mission(BallisticSolverFactory::createBallisticSolver(), TargetProviderFactory::createTargetProvider(Source::JSON, filePath));
    mission.init(missionConfig, bomb);

    do {
        mission.step();    
    } while (mission.hasNext());
    
    
    return 0;
}
