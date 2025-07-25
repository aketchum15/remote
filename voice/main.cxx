#include <array>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "inc/common.hxx"
#include "inc/recorder.hxx"
#include "inc/udpSender.hxx"

int32_t packet_size = 0;
std::condition_variable cv;
std::mutex mtx;

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
        }

        packet_size = 0;
    }
}

int main(void) {
    std::array<uint8_t, MAX_PACKET_SIZE> buf = {0};

    std::thread record_thread(recorder, std::ref(buf));
    std::thread send_thread(sender, std::ref(buf));

    record_thread.join();
    send_thread.join();
}
