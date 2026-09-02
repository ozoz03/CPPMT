#pragma once
#include "drone_link.h"

namespace autopilot {

struct Point { float x, y; };

// Час падіння боєприпасу (розв'язок кубічного рівняння методом Кардано).
// Портовано з homework_08/src/Utility.cpp::getTimeByCardano.
float timeByCardano(float mass, float drag, float lift, float altitude, float speed);

// Горизонтальна відстань, яку боєприпас пролітає за час падіння `time`.
// Портовано з homework_08/src/Utility.cpp::getDistanceByTime.
float distanceByTime(float time, float mass, float drag, float lift, float speed);

struct Engagement {
    Point predictedTarget; // де буде ціль у момент влучання
    Point dropPoint;       // де має бути дрон у момент скиду
    float horizon;         // сумарний час до влучання (політ дрона до dropPoint + падіння)
    bool  valid;           // false, якщо дрон вже проскочив точку скиду (D <= distance)
};

// Ітераційний пошук точки скиду: горизонт = час_падіння + час_польоту_дрона_до_dropPoint,
// а час_польоту сам залежить від горизонту (ціль встигає відʼїхати) — тож шукаємо
// нерухому точку (fixed point). Портовано й виправлено з
// homework_08/src/Utility.cpp::getBalisticResult (там баг: горизонт брався лише
// як час падіння, без урахування часу підльоту дрона).
Engagement planEngagement(Point dronePos, float droneSpeed,
                           Point targetPos, Point targetVelocity,
                           float bombFallTime, float bombHDistance);

} // namespace autopilot
