#include "inc/udpSender.hxx"
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

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

void udpSender::send(const std::vector<uint8_t> &buf) {

    size_t sent = sendto(this->sockfd, buf.data(), buf.size(), 0, reinterpret_cast<sockaddr*>(&destAddr), sizeof(destAddr));
    if (sent != buf.size()) {
        //TODO: error
    }
};
