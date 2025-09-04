#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <ranges>

template<typename T> 
class ThreadSafeQueue: private std::queue<T> {
 
    // No need for constructor destructor because of RAII kinda cool
    public:
        void push(T &data) {
            // another thing that is RAII
            std::lock_guard<std::mutex> lock(mtx);
            std::queue<T>::push(data);
            cv.notify_one();
        }

        T pop() {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&](){ return !std::queue<T>::empty(); });
            T ret = std::queue<T>::front();
            std::queue<T>::pop();
            lock.unlock();
            return ret;
        }

        //TODO: This will compile time error if there is no way to convert the range to the type of the queue (T)
        //would prefer it to be a little more type safe and not do coercion
        template<std::ranges::range R>
        void push_range(R&& rg) {
            std::lock_guard<std::mutex> lock(mtx);
            //TOD: move? move semantics ?
            std::queue<T>::push_range(rg);
            cv.notify_one();
        }

        template<std::ranges::range R>
        void pop_range_into(R& out, size_t n) {
            std::lock_guard<std::mutex> lock(mtx);

            auto size = std::min(std::queue<T>::size(), n);


        };

        bool empty() {
            std::lock_guard<std::mutex> lock(mtx);

            return std::queue<T>::empty();
        }

        size_t size() {
            std::lock_guard<std::mutex> lock(mtx);

            return std::queue<T>::size();
        };

    private:

        std::mutex mtx;
        std::condition_variable cv;
    

};

