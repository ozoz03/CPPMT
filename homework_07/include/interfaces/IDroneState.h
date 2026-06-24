#pragma once
#include "MissionContext.h"
#include <memory>

class IDroneState {
public:
    // IDroneState() = default;
    virtual ~IDroneState() = default;
 
    // Виконати логіку стану, повернути наступний стан.
    // Якщо стан не змінився — повернути nullptr
    // (головний цикл залишить поточний).
    virtual std::unique_ptr<IDroneState>
        execute(MissionContext& ctx) = 0;
 
    virtual const char* name() const = 0;
};
