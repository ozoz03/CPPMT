#pragma once
#include "DronePhase.h"

struct DroneCommand {
    DronePhase state;   // новий режим руху (STOPPED/ACCELERATING/DECELERATING/TURNING/MOVING)
    // Кутова швидкість, рад/с (знак = напрямок). Застосовується у будь-якому режимі:
    // у TURNING — поворот на місці, у ACCELERATING/MOVING — корекція курсу на ходу.
    float angleSpeed;
};
