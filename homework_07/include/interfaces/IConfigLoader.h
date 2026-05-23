#pragma once

class IConfigLoader{
    public:
    virtual load() = 0;
    virtual getConfig() = 0;
    virtual getAmmoParams() = 0;

    virtual ~IConfigLoader(){}
};