#include "StateAccelerating.h"
#include "StateMoving.h"
#include "StateDecelerating.h"
#include <iostream>


std::unique_ptr<IDroneState> StateAccelerating::execute(MissionContext& ctx)
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