#include <memory>
#include "MissionContext.h"
#include "StateTurning.h"
#include "StateStopped.h"
#include "StateAccelerating.h"
#include "DronePhase.h"
#include "Utility.h"
#include <cmath>

// ДЗ18: стани більше не інтегрують рух — лише вирішують режим і кладуть команду
// у ctx.command. Інтегрує фізика (DronePhysics) у власному потоці.
std::unique_ptr<IDroneState> StateStopped::execute(MissionContext& ctx) {
    float delta = normalizeAngle(ctx.desiredDir - ctx.droneContext.droneDirection);

    if (std::fabs(delta) > ctx.cfg.turnThreshold) {
        ctx.turnRemaining = std::fabs(delta) / ctx.cfg.angularSpeed;
        float turn = (delta > 0 ? 1.0f : -1.0f) * ctx.cfg.angularSpeed;
        ctx.command = {TURNING, turn};
        return std::make_unique<StateTurning>();
    }
    ctx.command = {ACCELERATING, 0.0f};
    return std::make_unique<StateAccelerating>();
}
