#pragma once
#include "IConfigLoader.h"
#include "MissionBuilder.h"
#include <string>
#include <iostream>


class JsonConfigLoader : public IConfigLoader {
public: 
    JsonConfigLoader() : IConfigLoader() {
        std::cout << "JsonConfigLoader default constructor called" << std::endl;
    };
    JsonConfigLoader(const std::string filename) : IConfigLoader(filename) {
        this->filename = filename;
        std::cout << "JsonConfigLoader created with file: " << filename << std::endl;
        load();
    };
    ~JsonConfigLoader() override = default;
    MissionConfig getConfig() override;
    AmmoParams getAmmoParams() override;
private:
    std::string filename;
    MissionBuilder configBuilder;
    AmmoParams ammoParams;
    void loadAmmoParams(const std::string& ammoName);
    void load() override;
};