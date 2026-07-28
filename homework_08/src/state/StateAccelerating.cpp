#include "StateAccelerating.h"
#include "DronePhase.h"
#include "StateMoving.h"
#include "StateDecelerating.h"
#include "Utility.h"
#include <cmath>

std::unique_ptr<IDroneState> StateAccelerating::execute(MissionContext& ctx) {
    float delta = normalizeAngle(ctx.desiredDir - ctx.droneContext.droneDirection);

    // Великий доворот — гальмуємо і розвертаємось на місці.
    if (std::fabs(delta) > static_cast<float>(M_PI) / 2.0f) {
        ctx.command = {DECELERATING, 0.0f};
        return std::make_unique<StateDecelerating>();
    }

    // Ще не набрали швидкість — розганяємось, коригуючи курс на ходу.
    if (ctx.droneContext.droneSpeed < ctx.cfg.attackSpeed) {
        ctx.command = {ACCELERATING, steeringRate(delta, ctx.cfg)};
        return nullptr;
    }

    ctx.command = {MOVING, steeringRate(delta, ctx.cfg)};
    return std::make_unique<StateMoving>();
}
