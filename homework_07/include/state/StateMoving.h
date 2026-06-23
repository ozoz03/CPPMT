#pragma once
#include "IDroneState.h"
#include "MissionContext.h"
#include <memory.h>
#include "Utility.h"
#include "StateDecelerating.h"

class StateMoving : public IDroneState {
public:
    std::unique_ptr<IDroneState>
        execute(MissionContext& ctx) override
    {
        std::cout << "STATE: " << name() << std::endl;

        // update drone position
		doMove(ctx.droneContext, ctx.cfg);


        //  Decelerating (якщо потрібен розворот)
        float delta = normalizeAngle(
            ctx.desiredDir - ctx.targetDir);
 
        if (std::fabs(delta) > ctx.cfg.turnThreshold) {
            std::cout << "State changing to : StateDecelerating" << std::endl;
            return std::make_unique<StateDecelerating>();
        }
        // setting a max drone speed
        ctx.droneContext.droneSpeed = ctx.cfg.attackSpeed;
        std::cout << "Still moving..." << std::endl;
        return nullptr;
    }
    const char* name() const override {
        return "Moving";
    }
};


