#include <memory>
#include "StateStopped.h"
#include "StateMoving.h"
#include "IDroneState.h"
#include "MissionContext.h"
#include "StateDecelerating.h"


std::unique_ptr<IDroneState> StateMoving::execute(MissionContext& ctx) {
    std::cout << "STATE: " << name() << std::endl;

        // update drone position
		doMove(ctx.droneContext, ctx.cfg);


        //  Decelerating (якщо потрібен розворот)
        float delta = normalizeAngle(
            ctx.desiredDir - ctx.droneContext.droneDirection);
 
        if (std::fabs(delta) > ctx.cfg.turnThreshold) {
            std::cout << "State changing to : StateDecelerating" << std::endl;
            return std::make_unique<StateDecelerating>();
        }
        // setting a max drone speed
        ctx.droneContext.droneSpeed = ctx.cfg.attackSpeed;
        std::cout << "Still moving..." << std::endl;
        return nullptr;
    
}