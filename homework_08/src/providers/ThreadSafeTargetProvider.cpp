#include "ThreadSafeTargetProvider.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <thread>

// Завантаження приватних траєкторій (формат ДЗ3) з targets.json.
void ThreadSafeTargetProvider::load() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    nlohmann::json jsonData;
    file >> jsonData;

    targetCount_ = jsonData["targetCount"];
    timeSteps_ = jsonData["timeSteps"];

    trajectories_.assign(targetCount_, std::vector<Coord>(timeSteps_));
    for (int i = 0; i < targetCount_; i++) {
        for (int j = 0; j < timeSteps_; j++) {
            trajectories_[i][j].x = jsonData["targets"][i]["positions"][j]["x"];
            trajectories_[i][j].y = jsonData["targets"][i]["positions"][j]["y"];
        }
    }

    // Початковий знімок: вузол 0 і швидкість між вузлами 0 та 1.
    current_.assign(targetCount_, Target{});
    node_ = 0;
    advanceNode();
    std::cout << "ThreadSafeTargetProvider loaded " << targetCount_
              << " targets (" << timeSteps_ << " nodes) from " << filename << std::endl;
}

// Оновити поточний знімок для вузла node_ (позиція + швидкість).
// Викликається під час load() (без потоку) і у циклі run() (в межах критичної секції).
void ThreadSafeTargetProvider::advanceNode() {
    for (int i = 0; i < targetCount_; i++) {
        const Coord cur = trajectories_[i][node_];
        const Coord next = trajectories_[i][(node_ + 1) % timeSteps_];
        // Швидкість — скінченна різниця сусідніх вузлів, поділена на arrayTimeStep.
        current_[i].pos = cur;
        current_[i].velocity = {(next.x - cur.x) / arrayTimeStep_,
                                (next.y - cur.y) / arrayTimeStep_};
    }
}

void ThreadSafeTargetProvider::run() {
    const float period = arrayTimeStep_ / timeScale_;
    std::cout << "[provider " << std::this_thread::get_id()
              << "] thread started, sleeping until start signal" << std::endl;
    ready_.store(true);
    // Чекаємо сигнал start(): цілі не рухаються, поки решта системи ініціалізується.
    while (!started_.load() && !stop_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::cout << "[provider " << std::this_thread::get_id() << "] woke on start signal, running"
              << std::endl;

    long ticks = 0;
    while (!stop_.load()) {
        // Sleep for one step, then wake up and advance to the next trajectory node.
        std::this_thread::sleep_for(std::chrono::duration<float>(period));

        int nextNode = (node_ + 1) % timeSteps_;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            node_ = nextNode;
            advanceNode();  // лише копіювання/арифметика, без sleep під замком
        }
        // Heartbeat: proves the target thread keeps sleeping/waking each step.
        if (++ticks % 20 == 0) {
            std::cout << "[provider " << std::this_thread::get_id() << "] tick " << ticks
                      << ": slept " << period << "s -> woke, node=" << nextNode << "/" << timeSteps_
                      << std::endl;
        }
    }
    std::cout << "[provider " << std::this_thread::get_id() << "] got stop, exiting after " << ticks
              << " ticks" << std::endl;
}

Target ThreadSafeTargetProvider::getTarget(int index) const {
    std::lock_guard<std::mutex> lock(mtx_);
    if (index < 0 || index >= targetCount_) {
        throw std::out_of_range("Target index out of range");
    }
    return current_[index];  // копія
}

std::vector<Target> ThreadSafeTargetProvider::getTargets() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return current_;  // копія
}
