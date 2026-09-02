#pragma once
#include <string>

struct gpiod_chip;
struct gpiod_line_request;

namespace autopilot {

// START — піднімається в 1 одразу на старті й тримається (сигнал чекеру "готовий").
// DROP  — короткий імпульс (50-100 мс) у момент скиду, рівно один раз за політ.
//
// libgpiod v2 (request-based API): обидві лінії запитуються разом одним
// gpiod_line_request, тримаємо лише offset'и, щоб потім адресувати їх у ньому.
class GpioLink {
public:
    ~GpioLink();

    bool open(const std::string& chipName, unsigned startLine, unsigned dropLine);
    void close();

    void setStart(bool value);
    // Неблокуюче: лише виставляє рівень. Утримання й звільнення через паузу —
    // відповідальність викликача (див. main.cpp), щоб не зупиняти UART-цикл
    // усередині pulseDrop() блокуючим sleep().
    void setDrop(bool value);

private:
    gpiod_chip* chip_ = nullptr;
    gpiod_line_request* request_ = nullptr;
    unsigned startLine_ = 0;
    unsigned dropLine_ = 0;
};

} // namespace autopilot
