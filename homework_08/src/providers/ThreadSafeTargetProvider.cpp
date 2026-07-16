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
    ready_.store(true);
    // Чекаємо сигнал start(): цілі не рухаються, поки решта системи ініціалізується.
    while (!started_.load() && !stop_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    while (!stop_.load()) {
        // Спимо період кроку (масштабований), потім переходимо до наступного вузла.
        std::this_thread::sleep_for(
            std::chrono::duration<float>(arrayTimeStep_ / timeScale_));

        int nextNode = (node_ + 1) % timeSteps_;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            node_ = nextNode;
            advanceNode();  // лише копіювання/арифметика, без sleep під замком
        }
    }
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
