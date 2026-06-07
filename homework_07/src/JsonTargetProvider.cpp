#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "ITargetProvider.h"
#include "Target.h"
#include "Point.h"
#include <iostream>

class JsonTargetProvider: public ITargetProvider {
private:
    Target** targets;
    int targetCount;
public:
    ~JsonTargetProvider() {
        for (int i = 0; i < targetCount; i++) {
            delete[] targets[i];
        }
        delete[] targets;
    }

    JsonTargetProvider(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }   
        nlohmann::json jsonData;
        file >> jsonData;

	    targetCount = jsonData["targetCount"];
	    int timeSteps = jsonData["timeSteps"];
 
	    targets = new Target*[targetCount];
	    for (int i = 0; i < targetCount; i++) {
    	    targets[i] = new Target[timeSteps];
            
            // targets[i]->positions = new Point*[60];
		    for (int j = 0; j < timeSteps; j++) {
        	    targets[i]->positions[j].x = jsonData["targets"][i]["positions"][j]["x"];
        	    targets[i]->positions[j].y = jsonData["targets"][i]["positions"][j]["y"];
			    std::cout << "Loaded target " << i << " position at time step " << j << ": (" << targets[i]->positions[j].x << ", " << targets[i]->positions[j].y << ")" << std::endl;
		}
            
	}
    std::cout << "Successfully loaded " << targetCount << " targets from " << filePath << std::endl;

    }   
    int getTargetCount() override {
        return targetCount;
    }
    Target getTarget(int index) override {
        if (index < 0 || index >= targetCount) {
            throw std::out_of_range("Index out of range");
        }
        return *targets[index];
    }

    Target** getTargets() override {
        return targets;
    }
};