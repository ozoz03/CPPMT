#pragma once
#include "SourceType.h"
#include <memory>

class IBallisticSolver;
class IConfigLoader;
class ITargetProvider;

class ConfigLoaderFactory {
public:
    static std::unique_ptr<IConfigLoader> createConfigLoader(const std::string filename);
};

class TargetProviderFactory {
public:
    static std::unique_ptr<ITargetProvider> createTargetProvider(const Source sourceType,const std::string filename);
};

class BallisticSolverFactory {
public:
    static std::unique_ptr<IBallisticSolver> createBallisticSolver();    
};

// class MissionFactory {
// public:
//     static std::unique_ptr<IMission> createMission(); 
// };