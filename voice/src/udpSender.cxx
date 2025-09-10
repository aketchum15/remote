#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "udpSender.hxx"

udpSender::udpSender(std::string ip, const uint16_t port, ThreadSafeQueue &q): q{q} {

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&destAddr, 0, sizeof(destAddr));

    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &destAddr.sin_addr);
    //TODO: err
};

udpSender::~udpSender() {
    close(sockfd);
};

void udpSender::send() {

    std::array<uint8_t, max_packet_size> send_buf;
    auto size = q.pop_range_into(send_buf.begin(), max_packet_size);

    size_t sent = sendto(
            this->sockfd, 
            send_buf.data(), 
            size, 
            0, 
            reinterpret_cast<sockaddr*>(&destAddr), 
            sizeof(destAddr)
    );

    if (sent != max_packet_size) {
        //TODO: error
    }
};
