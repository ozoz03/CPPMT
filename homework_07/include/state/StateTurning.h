#pragma once
#include "IDroneState.h"
#include "MissionContext.h"
#include <memory>
#include "StateAccelerating.h"
#include <iostream>


class StateTurning : public IDroneState {
public:
    std::unique_ptr<IDroneState>
        execute(MissionContext& ctx) override;
    const char* name() const override {
        return "Turning";
    }
};


