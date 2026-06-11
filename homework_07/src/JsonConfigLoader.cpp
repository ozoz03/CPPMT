#include "MissionConfig.h"
#include "IConfigLoader.h"
#include "MissionBuilder.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include "config.hpp"

class JsonConfigLoader : public IConfigLoader {

public:     
    JsonConfigLoader(){
        std::cout << "JsonConfigLoader default constructor called" << std::endl;
    };
    JsonConfigLoader(const std::string filename) : IConfigLoader(filename)  {       
        this->filename = filename;
        std::cout << "JsonConfigLoader 1111 created with file: " << filename << std::endl;
        load();
    }
    
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

    void load() override{
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
        configBuilder.ammo(jsonData["ammo"].get<std::string>().c_str());	
        file.close();
        std::cout << "Config loaded successfully from " << filename << std::endl;
    };

    void loadAmmoParams(const std::string& ammoName) {
        std::ifstream file(DATA_DIR_PATH.data() + std::string("/ammo.json"));
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

