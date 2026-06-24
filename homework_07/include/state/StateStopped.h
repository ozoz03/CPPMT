#pragma once
#include "IDroneState.h"
#include "MissionContext.h"
#include <memory>
#include "Utility.h"
#include <cmath>
#include "StateTurning.h"
#include "StateAccelerating.h"
#include <iostream>


class StateStopped : public IDroneState {
public:
    std::unique_ptr<IDroneState>
        execute(MissionContext& ctx) override;
    const char* name() const override {
        return "Stopped";
    }
};


