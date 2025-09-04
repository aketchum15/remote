#pragma once
#include <algorithm>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <optional>
#include <queue>
#include <ranges>

class ThreadSafeQueue {

    public:
        void push(uint8_t byte) {
            {
                std::lock_guard<std::mutex> lock(mtx);
                q.push(byte);
                closed = false;
            }
            cv.notify_one();
        }

        template<std::ranges::range R>
        void push_range(R &&rg){
            {
                std::lock_guard<std::mutex> lock(mtx);
                q.push_range(rg);
                closed = false;
            }
            cv.notify_one();
        }

        uint8_t front() {
            std::unique_lock<std::mutex> lock(mtx);
            if (q.empty())
                cv.wait(lock);

            return q.front();
        }

        std::optional<uint8_t> pop() {
            std::unique_lock<std::mutex> lock(mtx);

            cv.wait(lock, [&] { return !q.empty() || closed; });
            if (q.empty())
                return std::nullopt;

            auto out = q.front();
            q.pop();
            return out;
        }

        template<typename C>
        size_t pop_range_into(std::back_insert_iterator<C> &&it, size_t n) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] {return !q.empty() || closed;});

            auto min = std::min(q.size(), n);
            for(size_t i = 0; i < min; i++) {
                it = q.front();
                q.pop();
            }
            return min;
        }

        size_t size() {
            std::lock_guard<std::mutex> lock(mtx);
            return q.size();
        }

        bool empty() {
            std::lock_guard<std::mutex> lock(mtx);
            return q.empty();
        }

        bool is_closed() {
            std::lock_guard<std::mutex> lock(mtx);
            return closed;
        }

        void close() {
            std::lock_guard<std::mutex> lock(mtx);
            closed = true;
            cv.notify_one();
        }

    private:
        std::queue<uint8_t> q;
        std::mutex mtx;
        std::condition_variable cv;
        bool closed;
};
