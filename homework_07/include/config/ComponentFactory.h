#pragma once
#include "IConfigLoader.h"
#include "ITargetProvider.h"
#include "SourceType.h"
#include <memory>

class ConfigLoaderFactory {
public:
    static std::unique_ptr<IConfigLoader> createConfigLoader(const std::string filename);
};

class TargetProviderFactory {
public:
    static std::unique_ptr<ITargetProvider> createTargetProvider(const Source sourceType,const std::string filename);
};