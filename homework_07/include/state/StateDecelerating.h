#pragma once
#include "IDroneState.h"
#include "MissionContext.h"
#include <memory.h>
#include "StateStopped.h"
#include "StateMoving.h"
#include <iostream>
#include <ostream>

class StateDecelerating : public IDroneState {
public:
    std::unique_ptr<IDroneState>
        execute(MissionContext& ctx) override
    {
        std::cout << "STATE: " << name() << std::endl;

        if (ctx.droneContext.droneSpeed > 0) {
            std::cout << "Still decelerating..." << std::endl;
            ctx.droneContext.droneSpeed--;
            return nullptr;
        }

        std::cout << "State changing to : StateStoped" << std::endl;
        return std::make_unique<StateStopped>();
    }
    const char* name() const override {
        return "Decelerating";
    }
};


