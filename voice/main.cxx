#include <array>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <wiringPi.h>

#include "inc/common.hxx"
#include "inc/recorder.hxx"
#include "inc/udpSender.hxx"

int32_t packet_size = 0;
std::condition_variable cv;
std::mutex mtx;


void gpio_listener(int32_t rec, int32_t notify) {
    const int DEBOUNCE_MS = 50;
    const int NO_TIMEOUT = -1;

    wiringPiSetupGpio();
    pinMode(rec, INPUT);

    pullUpDnControl(rec, PUD_UP);
    pinMode(notify, OUTPUT);
    digitalWrite(notify, LOW);

    bool toggle = false;
    auto callback = [notify](struct WPIWfiStatus status, void *user_data) {
        if (status.edgeType == INT_RISING_EDGE) {
            digitalWrite(notify, HIGH);
            // send signal
        } else {
            digitalWrite(notify, low);
            // send end signal
        }
    };
    wiringPiISR2(rec, INT_EDGE_BOTH, &callback, DEBOUNCE_MS, NULL);

    for (;;) {
        //while not die
    };

    wiringPiISRStop(rec);
    pinMode(notify, INPUT);
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

    std::thread record_thread(recorder, std::ref(buf));
    std::thread send_thread(sender, std::ref(buf));
    std::thread gpio_thread(gpio_listener, 4, 2);

    record_thread.join();
    send_thread.join();

}
