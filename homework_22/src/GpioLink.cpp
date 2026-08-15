#include "GpioLink.h"
#include <gpiod.h>
#include <unistd.h>
#include <cstdio>

namespace autopilot {

GpioLink::~GpioLink() { close(); }

bool GpioLink::open(const std::string& chipName, unsigned startLine, unsigned dropLine) {
    // gpiod_chip_open (v2) хоче шлях до файла пристрою, а не голу назву чипа.
    std::string path = chipName;
    if (path.empty() || path[0] != '/') {
        path = "/dev/" + path;
    }
    chip_ = gpiod_chip_open(path.c_str());
    if (!chip_) {
        perror("open gpiochip");
        return false;
    }

    startLine_ = startLine;
    dropLine_ = dropLine;
    unsigned int offsets[2] = {startLine, dropLine};

    gpiod_line_settings* settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);

    gpiod_line_config* lineConfig = gpiod_line_config_new();
    gpiod_line_config_add_line_settings(lineConfig, offsets, 2, settings);

    gpiod_request_config* reqConfig = gpiod_request_config_new();
    gpiod_request_config_set_consumer(reqConfig, "drone");

    request_ = gpiod_chip_request_lines(chip_, reqConfig, lineConfig);

    // Налаштування копіюються всередину під час запиту — самі об'єкти більше не потрібні.
    gpiod_request_config_free(reqConfig);
    gpiod_line_config_free(lineConfig);
    gpiod_line_settings_free(settings);

    if (!request_) {
        perror("gpiod_chip_request_lines");
        return false;
    }
    return true;
}

void GpioLink::close() {
    if (request_) { gpiod_line_request_release(request_); request_ = nullptr; }
    if (chip_) { gpiod_chip_close(chip_); chip_ = nullptr; }
}

void GpioLink::setStart(bool value) {
    if (!request_) return;
    gpiod_line_request_set_value(request_, startLine_,
        value ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE);
}

void GpioLink::setDrop(bool value) {
    if (!request_) return;
    gpiod_line_request_set_value(request_, dropLine_,
        value ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE);
}

} // namespace autopilot
