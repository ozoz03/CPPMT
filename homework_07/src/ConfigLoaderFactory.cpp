#include "ConfigLoaderFactory.h"
#include "JsonConfigLoader.h"
#include <stdexcept>
#include <memory>

// std::unique_ptr<IConfigLoader> ConfigLoaderFactory::createConfigLoader(const std::string& filename) {
//         if(filename.substr(filename.find_last_of(".") + 1) == "json"){
//             return std::make_unique<JsonConfigLoader>(filename);
//         }
//         throw std::runtime_error("Unsupported file format: " + filename);

// };

std::unique_ptr<IConfigLoader> ConfigLoaderFactory::createConfigLoader(const std::string filename){
        if(filename.substr(filename.find_last_of(".") + 1) == "json"){
            return std::make_unique<JsonConfigLoader>("filename");
        }
        throw std::runtime_error("Unsupported file format: " + filename);
};