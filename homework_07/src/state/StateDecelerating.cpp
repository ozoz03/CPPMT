#include <memory>
#include "StateDecelerating.h"
#include "IDroneState.h"
#include "MissionContext.h"


std::unique_ptr<IDroneState> StateDecelerating::execute(MissionContext& ctx) {
    std::cout << "STATE: " << name() << std::endl;

    if (ctx.droneContext.droneSpeed > 0) {
        std::cout << "Still decelerating..." << std::endl;
        ctx.droneContext.droneSpeed--;
        std::cout << "Drone speed: " << ctx.droneContext.droneSpeed << " Attack speed: " << ctx.cfg.attackSpeed << std::endl;
        return nullptr;
    }

    std::cout << "State changing to : StateStoped" << std::endl;
    return std::make_unique<StateStopped>();
}