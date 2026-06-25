#pragma once
#include <string>

struct MissionConfig;
struct AmmoParams;


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

