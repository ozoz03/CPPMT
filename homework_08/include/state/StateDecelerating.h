#pragma once
#include "IDroneState.h"
#include "MissionContext.h"
#include <memory>
#include "StateStopped.h"
#include "StateMoving.h"
#include <iostream>
#include <ostream>

class StateDecelerating : public IDroneState {
public:
    std::unique_ptr<IDroneState>
        execute(MissionContext& ctx) override;
        
    const char* name() const override {
        return "Decelerating";
    }
};


