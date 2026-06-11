#pragma once
#include "MissionConfig.h"
#include "AmmoParams.h"
#include <string>

class IConfigLoader{
    public:
    virtual void load(const std::string& filename) = 0;
    virtual MissionConfig getConfig() = 0;
    virtual AmmoParams getAmmoParams() = 0;

    virtual ~IConfigLoader(){}
};