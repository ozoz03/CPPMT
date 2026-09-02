#include "UartLink.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <cerrno>

namespace autopilot {

UartLink::~UartLink() { close(); }

bool UartLink::open(const std::string& device) {
    fd_ = ::open(device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd_ < 0) {
        perror("open uart");
        return false;
    }
    termios tio{};
    tcgetattr(fd_, &tio);
    cfmakeraw(&tio);
    cfsetispeed(&tio, B115200);
    cfsetospeed(&tio, B115200);
    tio.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd_, TCSANOW, &tio);
    return true;
}

void UartLink::close() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

void UartLink::poll(const PacketHandler& onPacket) {
    if (fd_ < 0) return;
    uint8_t buf[256];
    int n = read(fd_, buf, sizeof(buf));
    if (n <= 0) return; // немає даних (EAGAIN) або помилка — нічого не робимо

    uint8_t type = 0, len = 0, payload[260];
    for (int i = 0; i < n; ++i) {
        if (parser_.feed(buf[i], type, payload, len)) {
            onPacket(type, payload, len);
        }
    }
}

void UartLink::sendControl(float accel, float turnRate) {
    if (fd_ < 0) return;
    dlink::Control c{accel, turnRate};
    uint8_t out[64];
    size_t m = dlink::encode(dlink::PKT_CONTROL, &c, sizeof(c), out);

    // fd_ відкритий O_NONBLOCK — write() може повернути -1/EAGAIN (буфер порту
    // тимчасово повний, напр. під час сплеску AMMO+CONFIG+TARGET на старті) або
    // записати лише частину байтів. Ігнорувати результат означало мовчки губити
    // CONTROL-пакети саме в моменти найбільшого навантаження на лінію.
    size_t sent = 0;
    while (sent < m) {
        ssize_t n = write(fd_, out + sent, m - sent);
        if (n > 0) {
            sent += static_cast<size_t>(n);
        } else if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
            continue; // тимчасово неможливо записати — пробуємо ще раз негайно
        } else {
            break; // реальна помилка порту — далі нічого не вдіємо цього такту
        }
    }
}

} // namespace autopilot
