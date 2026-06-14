#include "ComponentFactory.h"
#include "IBallisticSolver.h"
#include "JsonConfigLoader.h"
#include "JsonTargetProvider.h"
#include "AnalyticalSolver.h"
#include "SourceType.h"
#include <stdexcept>
#include <memory>

std::unique_ptr<IConfigLoader> ConfigLoaderFactory::createConfigLoader(const std::string filename){
        if(filename.substr(filename.find_last_of(".") + 1) == "json"){
            return std::make_unique<JsonConfigLoader>(filename);
        }
        throw std::runtime_error("Unsupported file format: " + filename);
};

std::unique_ptr<ITargetProvider> TargetProviderFactory::createTargetProvider(const Source sourceType,const std::string filename){
        if(sourceType == Source::JSON){
            return std::make_unique<JsonTargetProvider>(filename);
        }
        throw std::runtime_error("Unsupported file format: " + filename);
};

std::unique_ptr<IBallisticSolver> BallisticSolverFactory::createBallisticSolver() {
    return std::make_unique<AnalyticalSolver>();
};