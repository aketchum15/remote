#include <string>
#include <thread>
#include <gpiod.hpp>

#include "recorder.hxx"
#include "udpSender.hxx"
#include "gpioListener.hxx"


int main(void) {
    ThreadSafeQueue q;

    gpioListener g(4, 2);
    Recorder r("hw:Device,0", q);
    udpSender s("127.0.0.1", 8888, q);


    auto sender = [&]{
        for (;;) 
            s.send();
    };

    auto recorder = [&]{
        for (;;) {
            g.getActive().wait(false);
            while(g.getActive().load()) {
                r.record();
            }
            r.close();
        }
    };

    std::thread record_thread(recorder);
    std::thread send_thread(sender);
    std::thread gpio_thread(&gpioListener::start, &g);

    record_thread.join();
    send_thread.join();
    gpio_thread.join();
}
