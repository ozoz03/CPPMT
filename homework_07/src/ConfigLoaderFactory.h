#pragma once
#include "IConfigLoader.h"
#include <memory>

class ConfigLoaderFactory {
public:
    static std::unique_ptr<IConfigLoader> createConfigLoader(const std::string filename);
};