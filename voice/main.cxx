#include <array>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <sys/poll.h>
#include <thread>
#include <gpiod.hpp>

#include "inc/common.hxx"
#include "inc/recorder.hxx"
#include "inc/udpSender.hxx"

int32_t packet_size = 0;
std::condition_variable cv;
std::mutex mtx;

std::atomic<bool> is_held{false};

void gpio_listener(std::string chip_path, int32_t btn, int32_t led) {
    const auto DEBOUNCE_MS = std::chrono::milliseconds(10);
    auto chip = ::gpiod::chip(chip_path);

    // This is crazy and awesome 
    auto btn_request = chip.prepare_request()
        .set_consumer("button consumer")
        .add_line_settings(
            btn,
            ::gpiod::line_settings()
                .set_direction(::gpiod::line::direction::INPUT)
                .set_edge_detection(::gpiod::line::edge::BOTH)
                .set_bias(::gpiod::line::bias::PULL_UP)
                .set_debounce_period(DEBOUNCE_MS)
        )
    .do_request();


    auto led_request = chip.prepare_request()
        .set_consumer("led consumer")
        .add_line_settings(
            led,
            ::gpiod::line_settings()
                .set_direction(::gpiod::line::direction::OUTPUT))
    .do_request();

    ::gpiod::edge_event_buffer buffer(1);
    for (;;) {
        btn_request.read_edge_events(buffer);
        for (const auto &event : buffer) {
            switch (event.type()) {
                case ::gpiod::edge_event::event_type::RISING_EDGE:
                    led_request.set_value(led, ::gpiod::line::value::ACTIVE);
                    //signal start
                break;
                case ::gpiod::edge_event::event_type::FALLING_EDGE:
                    led_request.set_value(led, ::gpiod::line::value::INACTIVE);
                    //signal stop
                break;
            }
        }
    }
};

void recorder(std::array<uint8_t, MAX_PACKET_SIZE>& buf) {

    Recorder r("hw:Device,0");

    r.init();
    for (;;) {

        {
            std::lock_guard<std::mutex> lock(mtx);
            packet_size = r.record(buf);
        }

        cv.notify_one();
    }

}

void sender(std::array<uint8_t, MAX_PACKET_SIZE>& buf) {
    udpSender sender("192.168.0.187", 8888);

    for (;;) {
        {
            std::unique_lock<std::mutex> lock(mtx);

            cv.wait(lock, [] { return packet_size > 0; });

            sender.send(buf, packet_size);
            std::cout << "sent " << packet_size << " bytes";
        }

        packet_size = 0;
    }
}

int main(void) {
    std::array<uint8_t, MAX_PACKET_SIZE> buf = {0};

    //std::thread record_thread(recorder, std::ref(buf));
    //std::thread send_thread(sender, std::ref(buf));
    std::thread gpio_thread(gpio_listener, "/dev/gpiochip0", 4, 2);
    //gpio_listener("/dev/gpiochip0", 4, 2);

    //record_thread.join();
    //send_thread.join();
    gpio_thread.join();

}
