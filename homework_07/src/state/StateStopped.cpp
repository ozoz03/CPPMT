#include <memory>
#include "MissionContext.h"
#include "StateTurning.h"
#include "StateStopped.h"
#include <iostream>

std::unique_ptr<IDroneState>StateStopped::execute(MissionContext& ctx)
        {
        std::cout << "STATE: " << name() << std::endl;
        
        float delta = normalizeAngle(ctx.desiredDir - ctx.droneContext.droneDirection);
 
        if (std::fabs(delta) > ctx.cfg.turnThreshold) {
            ctx.turnRemaining =
                std::fabs(delta) / ctx.cfg.angularSpeed;
            ctx.droneContext.droneDirection = ctx.desiredDir;
        
            std::cout << "State changing to : StateTurning" << std::endl;
            return std::make_unique<StateTurning>();
        }
        ctx.droneContext.droneDirection = ctx.desiredDir;
        
        std::cout << "State changing to : StateAccelerating" << std::endl;
        return std::make_unique<StateAccelerating>();
    }