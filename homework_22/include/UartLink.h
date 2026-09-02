#pragma once
#include "drone_link.h"
#include <string>
#include <functional>

namespace autopilot {

class UartLink {
public:
    ~UartLink();

    // Порт — "/tmp/ttyA" (sim) або "/dev/ttyAMA1" (плата). Налаштовує 115200 8N1 raw,
    // неблокуюче читання (O_NONBLOCK).
    bool open(const std::string& device);
    void close();

    using PacketHandler = std::function<void(uint8_t type, const uint8_t* payload, uint8_t len)>;

    // Прочитати все, що зараз доступно (неблокуюче), згодувати інкрементальному
    // парсеру, викликати onPacket для кожного зібраного валідного кадру.
    void poll(const PacketHandler& onPacket);

    void sendControl(float accel, float turnRate);

private:
    int fd_ = -1;
    dlink::Parser parser_;
};

} // namespace autopilot
