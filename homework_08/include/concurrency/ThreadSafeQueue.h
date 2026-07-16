#pragma once
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

// ДЗ18: власний потокобезпечний шаблонний контейнер для команд фізиці.
// Один продюсер (MissionProcessor) кладе команди, один консюмер (DronePhysics)
// вичерпує їх неблокуюче на своєму кроці інтегрування.
template <typename T>
class ThreadSafeQueue {
public:
    void push(const T& value) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            queue_.push(value);
        }
        cv_.notify_one();
    }

    // Неблокуюче зчитування: повертає std::nullopt, якщо черга порожня.
    std::optional<T> tryPop() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (queue_.empty()) {
            return std::nullopt;
        }
        T value = queue_.front();
        queue_.pop();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.empty();
    }

private:
    mutable std::mutex mtx_;
    // Поки не використовується: consumer читає неблокуюче через tryPop() і ніколи
    // не чекає. Лишено під можливий waitPop().
    std::condition_variable cv_;
    std::queue<T> queue_;
};
