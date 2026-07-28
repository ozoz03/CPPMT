#pragma once
#include "Target.h"
#include <string>
#include <vector>

// провайдер володіє позиціями/швидкостями цілей і рухає їх у власному потоці.
// Назовні віддає ЛИШЕ копії поточних значень — жодних посилань на внутрішні дані.
class ITargetProvider {
public:
    ITargetProvider() = default;
    ITargetProvider(const std::string filename) : filename(filename) {}
    virtual ~ITargetProvider() {}

    virtual int getTargetCount() const = 0;
    // Копії під м'ютексом:
    virtual Target getTarget(int index) const = 0;
    virtual std::vector<Target> getTargets() const = 0;

    // Лайфсайкл потоку:
    virtual void run() = 0;                  // тіло потоку
    virtual bool isThreadReady() const = 0;  // потік створено й готовий стартувати
    virtual void start() = 0;                // сигнал почати рух цілей
    virtual void stop() = 0;                 // атомарний стоп-прапорець

protected:
    std::string filename;

private:
    virtual void load() = 0;
};
