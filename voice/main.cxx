#include "inc/recorder.hxx"
#include "inc/udpSender.hxx"
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

bool data_ready = false;
std::condition_variable cv;
std::mutex mtx;

void recorder(std::vector<uint8_t> &buf) {
    Recorder r;
    r.setSoundDevice("hw:Device,0");

    r.init();
    for (;;){

        {
            std::lock_guard<std::mutex> lock(mtx);
            r.record(buf);
        }
        std::cout << "record thread done\n";
    }

    data_ready = true;
    cv.notify_one();
}

void sender(std::vector<uint8_t> &buf) {
    udpSender sender("192.168.0.187", 8888);

    std::unique_lock<std::mutex> lock(mtx);

    cv.wait(lock, [] { return data_ready; });

    for (auto c : buf) {
        printf("%02X ", c);
    }
    printf("\n");
    sender.send(buf);

    data_ready = false;
}

int main(void) {
    std::vector<uint8_t> buf;
    buf.reserve(48000/5 * sizeof(int16_t));

    std::thread record_thread(recorder, std::ref(buf));
    std::thread send_thread(sender, std::ref(buf));

    record_thread.join();
    send_thread.join();
}
