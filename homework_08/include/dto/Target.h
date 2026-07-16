#pragma once
#include "Point.h"

// ДЗ18: Target більше не містить масиву координат траєкторії.
// Траєкторії — приватні дані ThreadSafeTargetProvider. Назовні провайдер
// віддає лише поточний знімок: позицію та швидкість цілі.
struct Target {
    Coord pos;       // поточна позиція цілі
    Coord velocity;  // поточна швидкість цілі (скінченна різниця вузлів / arrayTimeStep)
};
