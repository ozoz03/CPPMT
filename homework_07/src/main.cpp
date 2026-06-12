#include "config.hpp"
#include "ConfigLoaderFactory.h"
#include "IConfigLoader.h"
#include <iostream>

// ITargetProvider* createProvider(SourceType sourceType, const std::string& filename){
//     if(sourceType == SourceType::JSON){
//         return new JsonTargetProvider(filename);
//     }
//     return nullptr;
// }

// class JsonConfigLoader : public IConfigLoader {
//     public:
//     explicit JsonConfigLoader(const std::string& filename) : filename(filename) {}
//     void load() override {
//         // Load the JSON file and parse it to fill the missionConfig and ammoParams
//         // This is a placeholder implementation
//         missionConfig = MissionConfig(); // Fill with actual data from JSON
//         ammoParams = AmmoParams(); // Fill with actual data from JSON
//     }
//     MissionConfig getConfig() override {
//         return missionConfig;
//     }
//     AmmoParams getAmmoParams() override {
//         return ammoParams;
//     }
//     private:
//     std::string filename;
//     MissionConfig missionConfig;
//     AmmoParams ammoParams;
// };

// IConfigLoader* createConfigLoader(const std::string& filename){
//     if(filename.substr(filename.find_last_of(".") + 1) == "json"){
//         return new JsonConfigLoader(filename);
//     }
//     return nullptr;
// }

int main() {
    std::string fileName = DATA_DIR_PATH.data() + std::string("/config.json");
    std::cout << "Loading config from: " << fileName << std::endl;
    auto jsonConfigLoader = ConfigLoaderFactory::createConfigLoader(fileName);
    MissionConfig missionConfig = jsonConfigLoader->getConfig();
    AmmoParams bomb = jsonConfigLoader->getAmmoParams();

    // auto* provider = createProvider(SourceType::JSON, DATA_DIR_PATH.data() + std::string("/targets.json"));
    // auto analytical = createBallisticSolver();
    // auto* mission = createMission(analytical.get(), provider);

    // do {
        // mission->step();    
    // } while (mission->hasNext());
    
    // delete mission;
    // delete provider;
    // delete analytical;
    return 0;
}
