#pragma once
#include "DroneCommand.h"
#include "MissionConfig.h"
#include "SimStep.h"

struct MissionContext {
    int currentStepIndex;
    SimStep droneContext;   // знімок стану дрона з телеметрії фізики 
    MissionConfig cfg;
    float desiredDir;
    float turnRemaining;
    DroneCommand command;   
};
