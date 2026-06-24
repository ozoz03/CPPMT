#pragma once
#include "IDroneState.h"
#include "MissionContext.h"
#include <memory>

class StateMoving : public IDroneState {
public:
    StateMoving(){};
    ~StateMoving(){};

    std::unique_ptr<IDroneState> execute(MissionContext& ctx) override;

    const char* name() const override {
        return "Moving";
    }
};


