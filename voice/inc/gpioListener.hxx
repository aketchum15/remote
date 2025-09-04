#pragma once

#include <condition_variable>
#include <gpiod.hpp>
#include <optional>

class gpioListener {

    public:
        gpioListener(gpiod::line::offset, std::optional<gpiod::line::offset> = std::nullopt);
        ~gpioListener(void) {};
        void start(void);

        std::atomic<bool> &getActive() { return active; };
        std::condition_variable &getCV() { return cv; };

    private:
        void _run();
        
        gpiod::line::offset line_in;
        std::optional<gpiod::line::offset> line_out;
        std::unique_ptr<gpiod::line_request> request_in;
        std::optional<gpiod::line_request> request_out;
        std::condition_variable cv;
        std::atomic<bool> active;

        static constexpr std::chrono::milliseconds debounce = std::chrono::milliseconds(300);
};
