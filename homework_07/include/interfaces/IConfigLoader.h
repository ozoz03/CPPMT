#pragma once
#include <string>
#include "MissionConfig.h"
#include "AmmoParams.h"


class IConfigLoader {
    public:
        IConfigLoader() = default;
        IConfigLoader(const std::string filename) : filename(filename) {}
        virtual MissionConfig getConfig() = 0;
        virtual AmmoParams getAmmoParams() = 0;
        virtual ~IConfigLoader(){}
    protected:
        std::string filename;
    private:
        virtual void load() = 0;
};

