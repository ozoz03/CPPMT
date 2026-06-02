#pragma once
#include "Target.h"
#include <vector>

class ITargetProvider {
public:
    virtual int    getTargetCount() = 0;
    virtual Target getTarget(int index) = 0;
    virtual std::vector<Target> getTargets() const = 0;
    
    virtual ~ITargetProvider() {}
};

