#include <memory>
#include "StateDecelerating.h"
#include "IDroneState.h"
#include "MissionContext.h"
#include "StateStopped.h"
#include "DronePhase.h"

std::unique_ptr<IDroneState> StateDecelerating::execute(MissionContext& ctx) {
    // Ще рухаємось — тримаємо режим гальмування, доки фізика не зупинить дрон.
    if (ctx.droneContext.droneSpeed > 0.0f) {
        ctx.command = {DECELERATING, 0.0f};
        return nullptr;
    }

    ctx.command = {STOPPED, 0.0f};
    return std::make_unique<StateStopped>();
}
