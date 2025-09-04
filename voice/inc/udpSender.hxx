#pragma once

#include <arpa/inet.h>
#include <string>

#include "tsq.hxx"

class udpSender {

    public:
        udpSender(std::string, const uint16_t, ThreadSafeQueue &);
        ~udpSender();

        void init();
        void send();

    private:
        int sockfd;
        struct sockaddr_in destAddr;

        ThreadSafeQueue &q;

        static constexpr size_t max_packet_size = 4096;
};
