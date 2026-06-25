#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "ITargetProvider.h"
#include "Target.h"
#include "JsonTargetProvider.h"
#include <iostream>
#include <vector>


void JsonTargetProvider::load() {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }   
        nlohmann::json jsonData;
        file >> jsonData;

	    targetCount = jsonData["targetCount"];
	    int timeSteps = jsonData["timeSteps"];
 
	    std::vector<Target>targets(targetCount);
	    for (int i = 0; i < targetCount; i++) {
            
            // targets[i]->positions = new Point*[60];
		    for (int j = 0; j < timeSteps; j++) {
        	    targets[i].positions[j].x = jsonData["targets"][i]["positions"][j]["x"];
        	    targets[i].positions[j].y = jsonData["targets"][i]["positions"][j]["y"];
			    std::cout << "Loaded target " << i << " position at time step " << j << ": (" << targets[i].positions[j].x << ", " << targets[i].positions[j].y << ")" << std::endl;
		}
            
	}
    std::cout << "Successfully loaded " << targetCount << " targets from " << filename << std::endl;
};  
    
Target& JsonTargetProvider::getTarget(int index) {
    if (index < 0 || index >= targetCount) {
        throw std::out_of_range("Index out of range");
    }
    return targets[index];
};

std::vector<Target>& JsonTargetProvider::getTargets() {
    return targets;
};
