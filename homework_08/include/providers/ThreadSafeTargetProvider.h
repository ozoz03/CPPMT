#pragma once
#include "ITargetProvider.h"
#include "Point.h"
#include "Target.h"
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

// потік 1. Володіє траєкторіями цілей
// Власний потік кожні arrayTimeStep секунд (масштабованих timeScale) переходить
// до наступного вузла траєкторії із зацикленням і оновлює поточні позицію та
// швидкість під м'ютексом. Назовні — лише копії Target.
class ThreadSafeTargetProvider : public ITargetProvider {
public:
    ThreadSafeTargetProvider(const std::string filename, float arrayTimeStep, float timeScale)
        : ITargetProvider(filename), arrayTimeStep_(arrayTimeStep), timeScale_(timeScale) {
        this->filename = filename;
        load();
    }
    ~ThreadSafeTargetProvider() override { stop(); }

    int getTargetCount() const override { return targetCount_; }
    Target getTarget(int index) const override;
    std::vector<Target> getTargets() const override;

    void run() override;
    bool isThreadReady() const override { return ready_.load(); }
    void start() override { started_.store(true); }
    void stop() override { stop_.store(true); }

private:
    void load() override;
    void advanceNode();  // перерахувати знімок для поточного node_ (сам node_ не змінює)

    // Приватні траєкторії: trajectories_[target][node].
    std::vector<std::vector<Coord>> trajectories_;
    int targetCount_ = 0;
    int timeSteps_ = 0;
    int node_ = 0;  // поточний вузол траєкторії

    float arrayTimeStep_;
    float timeScale_;

    mutable std::mutex mtx_;          // захищає current_
    std::vector<Target> current_;     // поточний знімок (позиція + швидкість)

    std::atomic<bool> ready_{false};
    std::atomic<bool> started_{false};
    std::atomic<bool> stop_{false};
};
