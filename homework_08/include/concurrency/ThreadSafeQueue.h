#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

#include "DroneCommand.h"

// Один продюсер (MissionProcessor) кладе команди, один консюмер (DronePhysics)
// вичерпує їх неблокуюче на своєму кроці інтегрування.
class ThreadSafeQueue {
public:
    void push(const DroneCommand& value) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            queue_.push(value);
        }
        cv_.notify_one();
    }

    // Неблокуюче зчитування: повертає false, якщо черга порожня.
    bool tryPop(DroneCommand& out) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (queue_.empty()) {
            return false;
        }
        out = queue_.front();
        queue_.pop();
        return true;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.empty();
    }

private:
    mutable std::mutex mtx_;

    std::condition_variable cv_;
    std::queue<DroneCommand> queue_;
};
