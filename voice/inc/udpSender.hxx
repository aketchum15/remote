#pragma once

#include <arpa/inet.h>
#include <string>

#include "common.hxx"
#include "threadSafeQueue.hxx"

class udpSender {

    public:
        udpSender(std::string, const uint16_t);
        ~udpSender();

        void init();
        void send();

    private:
        int sockfd;
        struct sockaddr_in destAddr;

        ThreadSafeQueue<uint8_t> q;

};
