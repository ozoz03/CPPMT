#pragma once
#include "Point.h"

struct DroneTelemetry {
    Coord pos;               // поточна позиція
    Coord speed;             // вектор швидкості (vx, vy)
    float timeSecSinceStart; // час останнього оновлення фізики (для компенсації нерівних кроків в аутпуті)
    float direction;         // курс дрона, рад
};
