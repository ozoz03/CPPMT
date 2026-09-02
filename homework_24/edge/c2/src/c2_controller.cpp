#include "c2_controller.hpp"
#include "fc_link.hpp"     // MAVSDK обгортка, API описано у fc_link.hpp
#include "udp_socket.hpp"  // UDP прийом, API описано у udp_socket.hpp

#include <nlohmann/json.hpp>  // Розбiр JSON з точками маршруту вiд auto_stub

#include <fstream>
#include <iostream>
#include <string>

static constexpr uint16_t STUB_PORT = 14560;

static const char* state_name(C2State s) {
    switch (s) {
        case C2State::DISARMED:     return "DISARMED";
        case C2State::ARMED_HOLD:   return "ARMED_HOLD";
        case C2State::ARMED_GUIDED: return "ARMED_GUIDED";
        case C2State::ARMED_MANUAL: return "ARMED_MANUAL";
    }
    return "UNKNOWN";
}

struct C2Controller::Impl {
    C2State state = C2State::DISARMED;

    std::ofstream log_file;
    std::unique_ptr<UdpSocket> udp;
    std::unique_ptr<FcLink> fc;
    bool healthy_written = false;

    explicit Impl(uint16_t fc_port) {
        log_file.open("/var/log/c2/c2.log", std::ios::app);

        log("[C2] listening for auto_stub on UDP " + std::to_string(STUB_PORT));
        udp = std::make_unique<UdpSocket>(STUB_PORT);

        log("[C2] waiting for FC heartbeat on UDP " + std::to_string(fc_port) + " ...");
        fc = std::make_unique<FcLink>(fc_port);
        log("[C2] FC connected");
    }

    void log(const std::string& msg) {
        std::cout << msg << std::endl;
        if (log_file.is_open()) {
            log_file << msg << std::endl;
            log_file.flush();
        }
    }

    C2State compute_state() const {
        if (!fc->is_armed())
            return C2State::DISARMED;

        switch (fc->flight_mode()) {
            case FcLink::FlightMode::Guided: return C2State::ARMED_GUIDED;
            case FcLink::FlightMode::Manual: return C2State::ARMED_MANUAL;
            case FcLink::FlightMode::Hold:
            case FcLink::FlightMode::Unknown:
            default:
                return C2State::ARMED_HOLD;
        }
    }

    void transition(C2State next) {
        if (next != state) {
            log(std::string("[C2] ") + state_name(state) + " -> " + state_name(next));
            state = next;
        }
    }
};

C2Controller::C2Controller(uint16_t fc_port)
    : impl_(std::make_unique<Impl>(fc_port))
{
}

C2Controller::~C2Controller() = default;

void C2Controller::tick() {
    if (!impl_->healthy_written && impl_->fc->is_connected()) {
        std::ofstream("/tmp/c2_healthy");
        impl_->healthy_written = true;
    }

    impl_->transition(impl_->compute_state());

    char buf[512];
    ssize_t n = impl_->udp->recv(buf, sizeof(buf) - 1);
    if (n <= 0)
        return;
    buf[n] = '\0';

    try {
        const auto j = nlohmann::json::parse(buf, buf + n);
        const float north_m = j.at("north_m").get<float>();
        const float east_m  = j.at("east_m").get<float>();

        if (impl_->state == C2State::ARMED_GUIDED) {
            impl_->fc->go_to_ned(north_m, east_m);
            impl_->log("[C2] forward waypoint north_m=" + std::to_string(north_m) +
                       " east_m=" + std::to_string(east_m));
        } else {
            impl_->log(std::string("[C2] blocked waypoint (state=") +
                       state_name(impl_->state) + ")");
        }
    } catch (const nlohmann::json::exception& e) {
        impl_->log(std::string("[C2] error: invalid waypoint JSON: ") + e.what());
    }
}

C2State C2Controller::current_state() const {
    return impl_->state;
}
