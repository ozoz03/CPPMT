#pragma once
#include "IDroneState.h"
#include "MissionContext.h"
#include <memory.h>
#include "StateDecelerating.h"
#include "StateMoving.h"


class StateAccelerating : public IDroneState {
public:
    std::unique_ptr<IDroneState>
        execute(MissionContext& ctx) override
    {
        std::cout << "STATE: " << name() << std::endl;

        if (ctx.droneContext.droneSpeed < ctx.cfg.attackSpeed) {
            std::cout << "Still accelerating..." << std::endl;
            ctx.droneContext.droneSpeed++;
            return nullptr;
        } 

        float delta = normalizeAngle(
            ctx.desiredDir - ctx.targetDir);
 
        if (std::fabs(delta) > ctx.cfg.turnThreshold) {
            std::cout << "State changing to : StateDecelerating" << std::endl;
            return std::make_unique<StateDecelerating>();
        }
        
        std::cout << "State changing to : StateMoving" << std::endl;
        return std::make_unique<StateMoving>();
    }
    const char* name() const override {
        return "Accelerating";
    }
};


