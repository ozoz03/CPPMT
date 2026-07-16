#include <memory>
#include "MissionContext.h"
#include "StateTurning.h"
#include <cmath>
#include "Utility.h"

std::unique_ptr<IDroneState> StateTurning::execute(MissionContext& ctx)
    {
        std::cout << "STATE: " << name() << std::endl;

        // doTurn(targetsAngleDiff[currentTargetIndex], simSteps[currentStepIndex], currentTargetIndex, cfg);

        float delta = normalizeAngle(
            ctx.desiredDir - ctx.droneContext.droneDirection);
 
        if (std::fabs(delta) > ctx.cfg.turnThreshold) {
            ctx.turnRemaining =
                std::fabs(delta) / ctx.cfg.angularSpeed;
            
            std::cout << "Still turring on..." << std::endl;

            if (ctx.droneContext.droneDirection > ctx.desiredDir) {
                ctx.droneContext.droneDirection += ctx.cfg.angularSpeed;
                std::cout << "Turring on right" << std::endl;
            } else {
                ctx.droneContext.droneDirection -= ctx.cfg.angularSpeed;
                std::cout << "Turring on left" << std::endl;
            }
            return nullptr;
        }
        ctx.droneContext.droneDirection = ctx.desiredDir;
        return std::make_unique<StateAccelerating>();
    }