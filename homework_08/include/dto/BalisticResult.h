#pragma once
#include "Point.h"

struct BalisticResult {
    Point dropPoint;
    Point aimPoint;
    Point predictedTarget;
};