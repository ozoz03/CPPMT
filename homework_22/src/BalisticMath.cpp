#include "BalisticMath.h"
#include <cmath>
#include <algorithm>

namespace autopilot {

float timeByCardano(float mass, float drag, float lift, float altitude, float speed) {
    float a = drag * 9.81f * mass - 2 * drag * drag * lift * speed;
    float b = -3 * 9.81f * mass * mass + 3 * drag * lift * mass * speed;
    float c = 6 * mass * mass * altitude;
    float p = (-b * b) / (3 * a * a);
    float q = 2 * b * b * b / (27 * a * a * a) + c / a;
    float arg = 3 * q / (2 * p) * std::sqrt(-3 / p);
    if (arg < -1 || arg > 1) {
        return -1.0f; // модель непридатна для цих параметрів
    }
    float phi = std::acos(arg);
    float t = 2 * std::sqrt(-p / 3) * std::cos((phi + 4 * static_cast<float>(M_PI)) / 3) - b / (3 * a);
    return t;
}

float distanceByTime(float time, float mass, float drag, float lift, float speed) {
    float h = speed * time - time * time * drag * speed / (2 * mass) +
              +time * time * time * (6 * drag * 9.81f * lift * mass - 6 * drag * drag * (lift * lift - 1) * speed) / (36 * mass * mass) +
              +std::pow(time, 4) * (-6 * drag * drag * 9.81f * lift * (1 + lift * lift + std::pow(lift, 4)) * mass + 3 * drag * drag * drag * lift * lift * (1 + lift * lift) * speed + 6 * drag * drag * drag * std::pow(lift, 4) * (1 + lift * lift) * speed) / (36 * std::pow(1 + lift * lift, 2) * mass * mass * mass) +
              +std::pow(time, 5) * (3 * drag * drag * drag * 9.81f * lift * lift * lift * mass - 3 * std::pow(drag, 4) * lift * lift * (1 + lift * lift) * speed) / (36 * (1 + lift * lift) * std::pow(mass, 4));
    return h;
}

Engagement planEngagement(Point dronePos, float droneSpeed,
                           Point targetPos, Point targetVelocity,
                           float bombFallTime, float bombHDistance) {
    float predictedX = targetPos.x;
    float predictedY = targetPos.y;
    float D = 0.0f;
    float horizon = bombFallTime;
    constexpr int kMaxIterations = 20;
    constexpr float kEpsilon = 1e-3f;
    // Якщо |targetVelocity|/droneSpeed > 1 (типово: щойно після старту, поки дрон ще
    // не розігнався, а оцінка швидкості цілі ще шумна), ітерація розбігається замість
    // збіжності — горизонт летить у нескінченність за кілька кроків. Місія й так
    // триває не більше ~90с, тож стеля тут — і фізично виправдана, і рятує від inf/nan.
    constexpr float kMaxHorizon = 120.0f;
    for (int i = 0; i < kMaxIterations; ++i) {
        predictedX = targetPos.x + targetVelocity.x * horizon;
        predictedY = targetPos.y + targetVelocity.y * horizon;
        D = std::sqrt((predictedX - dronePos.x) * (predictedX - dronePos.x) +
                       (predictedY - dronePos.y) * (predictedY - dronePos.y));
        float droneFlightTime = (D > bombHDistance) ? (D - bombHDistance) / droneSpeed : 0.0f;
        float newHorizon = std::min(bombFallTime + droneFlightTime, kMaxHorizon);
        if (std::fabs(newHorizon - horizon) < kEpsilon) {
            horizon = newHorizon;
            break;
        }
        horizon = newHorizon;
    }

    Engagement result{};
    result.predictedTarget = {predictedX, predictedY};
    result.horizon = horizon;
    // D може зрівнятись з bombHDistance (чи трохи впасти нижче через похибку округлення)
    // саме в момент, коли дрон опиняється в точці скидання — це не "невалідна" ціль,
    // а сигнал скидати негайно. Невалідність лишаємо тільки для виродженого D≈0.
    result.valid = D > 1e-3f;
    if (result.valid) {
        float ratio = std::clamp((D - bombHDistance) / D, 0.0f, 1.0f);
        result.dropPoint = {dronePos.x + (predictedX - dronePos.x) * ratio,
                             dronePos.y + (predictedY - dronePos.y) * ratio};
    } else {
        result.dropPoint = dronePos;
    }
    return result;
}

} // namespace autopilot
