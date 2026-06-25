#include "MissionProcessor.h"
#include "config.hpp"
#include "ComponentFactory.h"
#include "IConfigLoader.h"
#include "StateStopped.h"
#include <iostream>

int main() {
    try {
        std::string fileName = DATA_DIR_PATH.data() + std::string("/config.json");
        std::cout << "Loading config from: " << fileName << std::endl;
        auto jsonConfigLoader = ConfigLoaderFactory::createConfigLoader(fileName);
        MissionConfig missionConfig = jsonConfigLoader->getConfig();
        AmmoParams bomb = jsonConfigLoader->getAmmoParams();

        std::string filePath = DATA_DIR_PATH.data() + std::string("/targets.json");
        Mission mission = Mission(BallisticSolverFactory::createBallisticSolver(), 
            TargetProviderFactory::createTargetProvider(Source::JSON, filePath),
            std::make_unique<StateStopped>());
        mission.init(missionConfig, bomb);   

    do {
        mission.step();    
    } while (mission.hasNext());
    }
    catch (const std::exception &exc) {
        std::cerr << exc.what();
    }     
    
    return 0;
}
