#pragma once
#include "IDroneState.h"
#include "MissionContext.h"
#include <memory.h>
#include "Utility.h"
#include <cmath>
#include "StateTurning.h"
#include "StateAccelerating.h"
#include <iostream>


class StateStopped : public IDroneState {
public:
    std::unique_ptr<IDroneState>
        execute(MissionContext& ctx) override
        {
        std::cout << "STATE: " << name() << std::endl;
        
        float delta = normalizeAngle(ctx.desiredDir - ctx.targetDir);
 
        if (std::fabs(delta) > ctx.cfg.turnThreshold) {
            ctx.turnRemaining =
                std::fabs(delta) / ctx.cfg.angularSpeed;
            ctx.targetDir = ctx.desiredDir;
        
            std::cout << "State changing to : StateTurning" << std::endl;
            return std::make_unique<StateTurning>();
        }
        ctx.targetDir = ctx.desiredDir;
        
        std::cout << "State changing to : StateAccelerating" << std::endl;
        return std::make_unique<StateAccelerating>();
    }
    const char* name() const override {
        return "Stopped";
    }
};


