#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "udpSender.hxx"

udpSender::udpSender(std::string ip, const uint16_t port) {

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

    return;
    
    /*
    std::array<uint8_t, MAX_PACKET_SIZE> send_buf;
    for (int i = 0; i < MAX_PACKET_SIZE; i++) {
        send_buf[i] = q.pop();
    };

    size_t sent = sendto(this->sockfd, buf.data(), size, 0, reinterpret_cast<sockaddr*>(&destAddr), sizeof(destAddr));
    if (sent != size) {
        //TODO: error
    }
    */
};
