#include "IBallisticSolver.h"
#include "Point.h"
#include "AmmoParams.h"

class AnalyticalSolver : public IBallisticSolver {
public:
    AnalyticalSolver() = default;
    virtual Point solve(float time, const Point& position, float velocity, const AmmoParams& ammo) override {
        // Простая модель: учитываем только гравитацию и сопротивление воздуха
        float g = 9.81f; // ускорение свободного падения
        float dragForce = ammo.drag * velocity * velocity; // сила сопротивления воздуха
        float liftForce = ammo.lift * velocity * velocity; // сила подъема

        // Уравнение движения: F = m * a => a = F / m
        float ax = -dragForce / ammo.mass; // ускорение по горизонтали
        float ay = -g + (liftForce / ammo.mass); // ускорение по вертикали

        // Обновляем позицию с учетом ускорения
        Point newPosition;
        newPosition.x = position.x + velocity * time + 0.5f * ax * time * time;
        newPosition.y = position.y + velocity * time + 0.5f * ay * time * time;

        return newPosition;
    };

    ~AnalyticalSolver() override {};   
};  