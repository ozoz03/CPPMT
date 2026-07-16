#pragma once
#include "DroneCommand.h"
#include "MissionConfig.h"
#include "SimStep.h"

struct MissionContext {
    int currentStepIndex;
    SimStep droneContext;   // ДЗ18: знімок стану дрона з телеметрії фізики (mission ним не володіє)
    MissionConfig cfg;
    float desiredDir;
    float turnRemaining;
    DroneCommand command;   // ДЗ18: команда, яку стейт-машина віддає фізиці цього кроку
};
