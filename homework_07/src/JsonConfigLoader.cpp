#include "MissionConfig.h"
#include "IConfigLoader.h"
#include "MissionBuilder.cpp"
#include <fstream>
#include <nlohmann/json.hpp>

class JsonConfigLoader : public IConfigLoader {
public:
    void load(const std::string& filename) override{
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        };
        nlohmann::json jsonData;
        file >> jsonData;
        configBuilder.startAt(jsonData["drone"]["position"]["x"], jsonData["drone"]["position"]["y"]);
        configBuilder.altitude(jsonData["drone"]["altitude"]);
        configBuilder.initialDir(jsonData["drone"]["initialDir"]);
		configBuilder.attackSpeed(jsonData["drone"]["attackSpeed"]);
		configBuilder.accelPath(jsonData["drone"]["accelPath"]);
		configBuilder.arrayTimeStep(jsonData["drone"]["arrayTimeStep"]);
		configBuilder.simTimeStep(jsonData["drone"]["simTimeStep"]);
		configBuilder.hitRadius(jsonData["drone"]["hitRadius"]);
		configBuilder.angularSpeed(jsonData["drone"]["angularSpeed"]);
		configBuilder.turnThreshold(jsonData["drone"]["turnThreshold"]);
        configBuilder.maxTargets(jsonData["drone"]["maxTargets"]);
        configBuilder.ammo(jsonData["ammo"].get<std::string>().c_str());	
        file.close();
    };
    
    MissionConfig getConfig() override {
        return configBuilder.build();
    };

    AmmoParams getAmmoParams() override {
        loadAmmoParams(configBuilder.build().ammoName);
        return ammoParams;
    };

private:
    MissionBuilder configBuilder;
    AmmoParams ammoParams;
    void loadAmmoParams(const std::string& ammoName) {
        std::ifstream file("ammo.json");
        if (!file.is_open()) {
            throw std::runtime_error("Could not open ammo file");
        };
        nlohmann::json jsonData;
        file >> jsonData;
        for (const auto& item : jsonData) {
            if (item["name"] == ammoName) {
                std::strncpy(ammoParams.name, item["name"].get<std::string>().c_str(), sizeof(ammoParams.name) - 1);
                ammoParams.mass = item["mass"];
                ammoParams.drag = item["drag"];
                ammoParams.lift = item["lift"];
                ammoParams.type = item["type"];
                break;
            }
            else {
                throw std::runtime_error("Ammo type not found: " + ammoName);
            }
        }
        file.close();            
    }
};