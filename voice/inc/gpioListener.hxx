#pragma once

#include <condition_variable>
#include <gpiod.hpp>
#include <optional>

class gpioListener {

    public:
        gpioListener(gpiod::line::offset, std::optional<gpiod::line::offset> = std::nullopt);
        ~gpioListener(void) {};
        void start(void);
    private:

        void _run();
        
        gpiod::line::offset line_in;
        std::optional<gpiod::line::offset> line_out;
        std::condition_variable cv;
        std::mutex mtx;
        const std::chrono::milliseconds debounce = std::chrono::milliseconds(300);
        std::unique_ptr<gpiod::line_request> request_in;
        std::optional<gpiod::line_request> request_out;
        bool active;
};
