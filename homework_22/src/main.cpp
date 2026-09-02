#include "drone_link.h"
#include "UartLink.h"
#include "GpioLink.h"
#include "Autopilot.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <chrono>
#include <thread>

using namespace dlink;
using namespace autopilot;

namespace {

struct Args {
    std::string uartDev;
    std::string gpiochip;
    unsigned startLine = 24;
    unsigned dropLine = 23;
};

bool parseArgs(int argc, char** argv, Args& out) {
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto next = [&](const char* name) -> const char* {
            if (i + 1 >= argc) {
                fprintf(stderr, "missing value for %s\n", name);
                return nullptr;
            }
            return argv[++i];
        };
        if (a == "--uart") {
            const char* v = next("--uart"); if (!v) return false;
            out.uartDev = v;
        } else if (a == "--gpiochip") {
            const char* v = next("--gpiochip"); if (!v) return false;
            out.gpiochip = v;
        } else if (a == "--start-line") {
            const char* v = next("--start-line"); if (!v) return false;
            out.startLine = static_cast<unsigned>(std::atoi(v));
        } else if (a == "--drop-line") {
            const char* v = next("--drop-line"); if (!v) return false;
            out.dropLine = static_cast<unsigned>(std::atoi(v));
        } else {
            fprintf(stderr, "unknown argument: %s\n", a.c_str());
            return false;
        }
    }
    if (out.uartDev.empty() || out.gpiochip.empty()) {
        fprintf(stderr, "usage: %s --uart <dev> --gpiochip <name> [--start-line N] [--drop-line N]\n", argv[0]);
        return false;
    }
    return true;
}

} // namespace

int main(int argc, char** argv) {
    Args args;
    if (!parseArgs(argc, argv, args)) {
        return 1;
    }

    UartLink uart;
    if (!uart.open(args.uartDev)) {
        return 1;
    }

    GpioLink gpio;
    if (!gpio.open(args.gpiochip, args.startLine, args.dropLine)) {
        return 1;
    }

    Autopilot pilot;
    uint32_t lastTelemetryTms = 0;

    bool dropTriggered = false;
    std::chrono::steady_clock::time_point dropAt{};

    bool dropPulseActive = false;
    std::chrono::steady_clock::time_point dropPulseStart{};
    constexpr auto kDropPulseDuration = std::chrono::milliseconds(500);

    // START = 1 одразу і тримаємо — сигнал чекеру, що ми готові.
    gpio.setStart(true);
    printf("[student] START asserted, waiting for telemetry on %s (gpiochip=%s start=%u drop=%u)\n",
           args.uartDev.c_str(), args.gpiochip.c_str(), args.startLine, args.dropLine);

    for (;;) {
        uart.poll([&](uint8_t type, const uint8_t* payload, uint8_t len) {
            switch (type) {
            case PKT_AMMO: {
                if (len != sizeof(AmmoCfg)) return;
                AmmoCfg ammo;
                std::memcpy(&ammo, payload, sizeof(ammo));
                pilot.setAmmo(ammo);
                printf("[student] AMMO: %s mass=%.2f drag=%.2f lift=%.2f hitR=%.2f targets=%u\n",
                       ammo.name, ammo.mass, ammo.drag, ammo.lift, ammo.hitRadius, ammo.nTargets);
                break;
            }
            case PKT_CONFIG: {
                if (len != sizeof(DroneCfg)) return;
                DroneCfg cfg;
                std::memcpy(&cfg, payload, sizeof(cfg));
                pilot.setConfig(cfg);
                printf("[student] CONFIG: attackSpeed=%.2f accelPath=%.2f angularSpeed=%.2f turnThreshold=%.3f timeStep=%.3f\n",
                       cfg.attackSpeed, cfg.accelerationPath, cfg.angularSpeed, cfg.turnThreshold, cfg.timeStep);
                break;
            }
            case PKT_TARGET: {
                if (len != sizeof(TargetPos)) return;
                TargetPos pos;
                std::memcpy(&pos, payload, sizeof(pos));
                pilot.updateTarget(pos, lastTelemetryTms);
                break;
            }
            case PKT_TELEMETRY: {
                if (len != sizeof(Telemetry)) return;
                Telemetry t;
                std::memcpy(&t, payload, sizeof(t));
                lastTelemetryTms = t.t_ms;

                Autopilot::Decision decision = pilot.tick(t);
                uart.sendControl(decision.control.accel, decision.control.turnRate);

                if (decision.hasEngagement) {
                    printf("[T] t=%6.2fs drone=(%.1f,%.1f) v=%.2f dir=%.2f state=%u"
                           " | tgt#%u pos=(%.1f,%.1f) vel=(%.2f,%.2f) hor=%.2f"
                           " | drop=(%.1f,%.1f) dist=%.2f"
                           " | ctrl a=%+.2f w=%+.2f\n",
                           t.t_ms / 1000.0f, t.x, t.y, t.speed, t.dir, t.state,
                           decision.engagedTargetId, decision.targetPos.x, decision.targetPos.y,
                           decision.targetVelocity.x, decision.targetVelocity.y, decision.horizon,
                           decision.dropPoint.x, decision.dropPoint.y, decision.distToDrop,
                           decision.control.accel, decision.control.turnRate);
                } else {
                    printf("[T] t=%6.2fs drone=(%.1f,%.1f) v=%.2f dir=%.2f state=%u | no engagement yet | ctrl a=%+.2f w=%+.2f\n",
                           t.t_ms / 1000.0f, t.x, t.y, t.speed, t.dir, t.state,
                           decision.control.accel, decision.control.turnRate);
                }

                if (decision.triggerDrop && !dropTriggered) {
                    // 500мс з запасом (чекер міг не встигати помітити коротший 80мс
                    // імпульс при періодичному опитуванні GPIO) — але без блокуючого
                    // sleep(), щоб не зупиняти відправку CONTROL саме в момент скиду.
                    // Звільнення лінії — нижче в головному циклі, за реальним часом.
                    gpio.setDrop(true);
                    dropPulseActive = true;
                    dropPulseStart = std::chrono::steady_clock::now();
                    dropTriggered = true;
                    dropAt = std::chrono::steady_clock::now();
                    printf("[student] DROP at t=%.2fs pos=(%.1f,%.1f)\n", t.t_ms / 1000.0f, t.x, t.y);
                }
                break;
            }
            default:
                break;
            }
        });

        if (dropPulseActive &&
            std::chrono::steady_clock::now() - dropPulseStart > kDropPulseDuration) {
            gpio.setDrop(false);
            dropPulseActive = false;
        }

        if (dropTriggered) {
            auto elapsed = std::chrono::steady_clock::now() - dropAt;
            if (elapsed > std::chrono::seconds(3)) {
                break; // дали чекеру час дочитати останні пакети й порахувати вердикт
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    return 0;
}
