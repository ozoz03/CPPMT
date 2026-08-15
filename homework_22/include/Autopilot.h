#pragma once
#include "drone_link.h"
#include "BalisticMath.h"
#include <unordered_map>
#include <cstdint>

namespace autopilot {

// Окремий від логіки наведення модуль керування: перетворює рішення "куди летіти
// і коли скидати" на нормовані команди UART (accel, turnRate) + сигнал DROP.
class Autopilot {
public:
    void setAmmo(const dlink::AmmoCfg& ammo);
    void setConfig(const dlink::DroneCfg& cfg);

    // Ціль не передає власну швидкість — оцінюємо її різницею послідовних позицій.
    // t_ms беремо з останньої отриманої телеметрії (окремого часу в PKT_TARGET немає).
    void updateTarget(const dlink::TargetPos& pos, uint32_t t_ms);

    struct Decision {
        dlink::Control control{0.0f, 0.0f};
        bool triggerDrop = false;

        // Діагностика для логів — що саме порахував autopilot цього такту.
        bool hasEngagement = false;
        uint8_t engagedTargetId = 0;
        Point targetPos{};
        Point targetVelocity{};
        Point predictedTarget{};
        Point dropPoint{};
        float horizon = 0.0f;
        float distToDrop = 0.0f;
    };

    // Викликається на кожен PKT_TELEMETRY. Повертає команду керування і, якщо настав
    // момент, прапорець на скид (спрацьовує рівно один раз за політ).
    Decision tick(const dlink::Telemetry& telemetry);

    bool hasDropped() const { return dropped_; }
    bool isReady() const { return ammoSet_ && configSet_; }

private:
    struct TargetState {
        Point pos{};
        uint32_t t_ms = 0;
        bool hasSample = false;
        bool hasVelocity = false;
        Point velocity{0.0f, 0.0f};
    };

    bool ammoSet_ = false;
    bool configSet_ = false;
    bool dropped_ = false;

    dlink::AmmoCfg ammo_{};
    dlink::DroneCfg cfg_{};

    std::unordered_map<uint8_t, TargetState> targets_;

    // Ціль фіксується один раз (перший такт, де є хоч один валідний прорахунок) і
    // далі не переглядається — інакше при близьких горизонтах кількох цілей вибір
    // "найвигіднішої" тремтить щотакту, і дрон не встигає впевнено вийти на курс.
    bool hasEngagedTarget_ = false;
    uint8_t engagedTargetId_ = 0;
};

} // namespace autopilot
