#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "ITargetProvider.h"
#include "Target.h"
#include "Point.h"

class JsonTargetProvider: public ITargetProvider {
public:
    JsonTargetProvider(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }   
        nlohmann::json jsonData;
        file >> jsonData;
        for (const auto& item : jsonData) {
            float x = item["x"];
            float y = item["y"];
            targets.emplace_back(Point(x, y));
        }
    }   
    int getTargetCount() override {
        return targets.size();
    }
    Target getTarget(int index) override {
        if (index < 0 || index >= int(targets.size())) {
            throw std::out_of_range("Index out of range");
        }
        return targets[index];
    }

    std::vector<Target> getTargets() const override {
        return targets;
    }
private:    
   std::vector<Target> targets;
};