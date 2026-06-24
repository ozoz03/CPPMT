#pragma once
#include "MissionConfig.h"
#include "SimStep.h"

struct MissionContext {
    int currentStepIndex;
    SimStep droneContext;
    MissionConfig cfg;
    float desiredDir;
    float turnRemaining;
    float targetDir;  
};