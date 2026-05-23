
#include "JsonTargetProvider.h"
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

class JsonTargetProvider: public TargetProvider {
public:
    JsonTargetProvider(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }   
        nlohmann::json jsonData;
        file >> jsonData;
        for (const auto& item : jsonData) {
            std::string name = item["name"];
            std::string type = item["type"];
            targets.emplace_back(name, type);
        }
    }   
    std::vector<Target> getTargets() const override {
        return targets;
    }
private:    std::vector<Target> targets;
};