#include <array>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <sys/poll.h>
#include <thread>
#include <gpiod.hpp>

#include "common.hxx"
#include "recorder.hxx"
#include "udpSender.hxx"
#include "gpioListener.hxx"

int32_t packet_size = 0;
std::condition_variable cv;
std::mutex mtx;


void recorder() {

    Recorder r("hw:Device,0");

    r.init();
    for (;;) {

        {
            std::lock_guard<std::mutex> lock(mtx);
            packet_size = r.record();
        }

        cv.notify_one();
    }

}

void sender() {
    udpSender sender("192.168.0.187", 8888);

    for (;;) {
        {
            std::unique_lock<std::mutex> lock(mtx);

            cv.wait(lock, [] { return packet_size > 0; });

            sender.send();
            std::cout << "sent " << packet_size << " bytes";
        }

        packet_size = 0;
    }
}

int main(void) {
    auto g = gpioListener(4, 2);

    std::thread record_thread(recorder);
    std::thread send_thread(sender);
    std::thread gpio_thread(&gpioListener::start, &g);

    record_thread.join();
    send_thread.join();
    gpio_thread.join();
}
