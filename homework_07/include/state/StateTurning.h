#pragma once
#include "IDroneState.h"
#include "MissionContext.h"
#include <memory.h>
#include "StateAccelerating.h"

const float gradus = 0.01745f;

class StateTurning : public IDroneState {
public:
    std::unique_ptr<IDroneState>
        execute(MissionContext& ctx) override
    {
        std::cout << "STATE: " << name() << std::endl;

        // doTurn(targetsAngleDiff[currentTargetIndex], simSteps[currentStepIndex], currentTargetIndex, cfg);

        float delta = normalizeAngle(
            ctx.desiredDir - ctx.targetDir);
 
        if (std::fabs(delta) > ctx.cfg.turnThreshold) {
            ctx.turnRemaining =
                std::fabs(delta) / ctx.cfg.angularSpeed;
            
            std::cout << "Still turring on..." << std::endl;

            if (ctx.targetDir > ctx.desiredDir) {
                ctx.targetDir += gradus;
                std::cout << "Turring on right" << std::endl;
            } else {
                ctx.targetDir -= gradus;
                std::cout << "Turring on left" << std::endl;
            }
            return nullptr;
        }
        ctx.targetDir = ctx.desiredDir;
        return std::make_unique<StateAccelerating>();
    }
    const char* name() const override {
        return "Turning";
    }
};


