#include <memory>
#include "MissionContext.h"
#include "StateTurning.h"
#include "DronePhase.h"
#include <cmath>
#include "Utility.h"

std::unique_ptr<IDroneState> StateTurning::execute(MissionContext& ctx) {
    float delta = normalizeAngle(ctx.desiredDir - ctx.droneContext.droneDirection);

    if (std::fabs(delta) > ctx.cfg.turnThreshold) {
        ctx.turnRemaining = std::fabs(delta) / ctx.cfg.angularSpeed;
        // Крутимося у бік найкоротшого довороту (знак delta після нормалізації).
        float turn = (delta > 0 ? 1.0f : -1.0f) * ctx.cfg.angularSpeed;
        ctx.command = {TURNING, turn};
        return nullptr;
    }
    ctx.turnRemaining = 0.0f;
    ctx.command = {ACCELERATING, 0.0f};
    return std::make_unique<StateAccelerating>();
}
