#include "Autopilot.h"
#include <cmath>
#include <algorithm>
#include <limits>

namespace autopilot {

namespace {
float clamp11(float v) { return std::max(-1.0f, std::min(1.0f, v)); }
float normalizeAngle(float angle) { return std::atan2(std::sin(angle), std::cos(angle)); }
} // namespace

void Autopilot::setAmmo(const dlink::AmmoCfg& ammo) {
    ammo_ = ammo;
    ammoSet_ = true;
}

void Autopilot::setConfig(const dlink::DroneCfg& cfg) {
    cfg_ = cfg;
    configSet_ = true;
}

void Autopilot::updateTarget(const dlink::TargetPos& pos, uint32_t t_ms) {
    TargetState& st = targets_[pos.id];
    Point newPos{pos.x, pos.y};
    if (st.hasSample) {
        float dt = (t_ms > st.t_ms) ? (t_ms - st.t_ms) / 1000.0f : 0.0f;
        if (dt > 1e-4f) {
            Point rawVelocity{(newPos.x - st.pos.x) / dt, (newPos.y - st.pos.y) / dt};

            // Перший вимір (чи будь-який з надто малим dt) може дати нефізично
            // велику швидкість — один такий викид, не обмежений, отруює EMA
            // і змушує ітерацію в planEngagement розбігатись (|vel|/droneSpeed > 1
            // на старті, поки дрон ще не розігнався).
            constexpr float kMaxTargetSpeed = 30.0f; // м/с, з запасом понад будь-яку реальну ціль
            float rawSpeed = std::sqrt(rawVelocity.x * rawVelocity.x + rawVelocity.y * rawVelocity.y);
            if (rawSpeed > kMaxTargetSpeed) {
                float scale = kMaxTargetSpeed / rawSpeed;
                rawVelocity.x *= scale;
                rawVelocity.y *= scale;
            }

            // EMA замість сирої різниці двох останніх точок — на криволінійній
            // траєкторії (еліпси/вісімки/лісажу) секанс між двома точками несе
            // помітний шум, який на довгому горизонті екстраполяції (~10-15с)
            // перетворюється на суттєву похibку прицілювання.
            constexpr float kSmoothing = 0.3f;
            if (st.hasVelocity) {
                st.velocity.x = kSmoothing * rawVelocity.x + (1.0f - kSmoothing) * st.velocity.x;
                st.velocity.y = kSmoothing * rawVelocity.y + (1.0f - kSmoothing) * st.velocity.y;
            } else {
                st.velocity = rawVelocity;
                st.hasVelocity = true;
            }
        }
    }
    st.pos = newPos;
    st.t_ms = t_ms;
    st.hasSample = true;
}

Autopilot::Decision Autopilot::tick(const dlink::Telemetry& telemetry) {
    Decision decision;
    if (!isReady() || targets_.empty()) {
        return decision; // немає ще даних — тримаємось нейтрально
    }

    Point dronePos{telemetry.x, telemetry.y};
    // Плануємо балістику під крейсерську швидкість (attackSpeed) — саме до неї
    // прямує regulator швидкості нижче, тож горизонт з часом сходиться до неї.
    float bombFallTime = timeByCardano(ammo_.mass, ammo_.drag, ammo_.lift, telemetry.z, cfg_.attackSpeed);
    float bombHDist = distanceByTime(bombFallTime, ammo_.mass, ammo_.drag, ammo_.lift, cfg_.attackSpeed);

    // Швидкість польоту дрона до точки скидання рахуємо від поточної (не цільової)
    // швидкості — контур керування реального часу, переплановуємо щотакту.
    float planningSpeed = std::max(telemetry.speed, 0.5f);

    bool haveBest = false;
    Engagement best{};

    if (hasEngagedTarget_) {
        // Ціль уже зафіксована — рахуємо тільки її, курс не тремтить між кандидатами.
        auto it = targets_.find(engagedTargetId_);
        if (it != targets_.end()) {
            Engagement eng = planEngagement(dronePos, planningSpeed, it->second.pos, it->second.velocity,
                                             bombFallTime, bombHDist);
            if (eng.valid) {
                best = eng;
                haveBest = true;
            }
        }
    }

    if (!haveBest) {
        // Перший такт з даними (чи втрачена зафіксована ціль) — обираємо найвигіднішу
        // і фіксуємось на ній на решту польоту.
        for (const auto& [id, st] : targets_) {
            Engagement eng = planEngagement(dronePos, planningSpeed, st.pos, st.velocity, bombFallTime, bombHDist);
            if (!eng.valid) continue;
            if (!haveBest || eng.horizon < best.horizon) {
                best = eng;
                haveBest = true;
                engagedTargetId_ = id;
            }
        }
        if (haveBest) {
            hasEngagedTarget_ = true;
        }
    }

    if (!haveBest) {
        // Усі цілі вже позаду точки скидання (чи щойно старт) — летимо прямо, розганяємось.
        float speedErr = cfg_.attackSpeed - telemetry.speed;
        decision.control.accel = clamp11(speedErr);
        decision.control.turnRate = 0.0f;
        return decision;
    }

    float desiredDir = std::atan2(best.dropPoint.y - dronePos.y, best.dropPoint.x - dronePos.x);
    float delta = normalizeAngle(desiredDir - telemetry.dir);
    float turnScale = std::max(cfg_.turnThreshold, 1e-3f);
    decision.control.turnRate = clamp11(delta / turnScale);

    float speedErr = cfg_.attackSpeed - telemetry.speed;
    decision.control.accel = clamp11(speedErr);

    float distToDrop = std::sqrt((dronePos.x - best.dropPoint.x) * (dronePos.x - best.dropPoint.x) +
                                  (dronePos.y - best.dropPoint.y) * (dronePos.y - best.dropPoint.y));

    decision.hasEngagement = true;
    decision.engagedTargetId = engagedTargetId_;
    decision.targetPos = targets_.at(engagedTargetId_).pos;
    decision.targetVelocity = targets_.at(engagedTargetId_).velocity;
    decision.predictedTarget = best.predictedTarget;
    decision.dropPoint = best.dropPoint;
    decision.horizon = best.horizon;
    decision.distToDrop = distToDrop;

    if (!dropped_) {
        float releaseTolerance = std::max(telemetry.speed * cfg_.timeStep, 0.5f);
        if (distToDrop <= releaseTolerance) {
            decision.triggerDrop = true;
            dropped_ = true;
        }
    }

    return decision;
}

} // namespace autopilot
