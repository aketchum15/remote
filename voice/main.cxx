#include "inc/recorder.hxx"
#include "inc/udpSender.hxx"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

bool data_ready = false;
std::condition_variable cv;
std::mutex mtx;

void recorder(std::vector<uint8_t> &buf) {
    Recorder r;

    {
        std::lock_guard<std::mutex> lock(mtx);
        r.init();
        r.record(buf);
    }

    data_ready = true;
    cv.notify_one();
}

void sender(std::vector<uint8_t> &buf) {
    udpSender sender("localhost", 8888);

    std::unique_lock<std::mutex> lock(mtx);

    cv.wait(lock, [] { return data_ready; });

    sender.send(buf);

    data_ready = false;
}

int main(void) {
    std::vector<uint8_t> buf;

    std::thread record_thread(recorder, std::ref(buf));
    std::thread send_thread(sender, std::ref(buf));

    record_thread.join();
    send_thread.join();
}
