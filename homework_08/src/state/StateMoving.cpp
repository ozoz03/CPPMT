#include <memory>
#include "StateMoving.h"
#include "IDroneState.h"
#include "MissionContext.h"
#include "StateDecelerating.h"
#include "DronePhase.h"
#include "Utility.h"
#include <cmath>

std::unique_ptr<IDroneState> StateMoving::execute(MissionContext& ctx) {
    float delta = normalizeAngle(ctx.desiredDir - ctx.droneContext.droneDirection);

    // Великий доворот (напр., зміна цілі) — гальмуємо і розвертаємось на місці.
    if (std::fabs(delta) > static_cast<float>(M_PI) / 2.0f) {
        ctx.command = {DECELERATING, 0.0f};
        return std::make_unique<StateDecelerating>();
    }

    // Інакше тримаємо швидкість і коригуємо курс на ходу (координований поворот).
    ctx.command = {MOVING, steeringRate(delta, ctx.cfg)};
    return nullptr;
}
