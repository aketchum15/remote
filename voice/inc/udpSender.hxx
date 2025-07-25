#include <arpa/inet.h>
#include <array>
#include <string>

#include "common.hxx"

class udpSender {

    public:
        udpSender(std::string, const uint16_t);
        ~udpSender();

        void init();
        void send(std::array<uint8_t, MAX_PACKET_SIZE> &, size_t);

    private:
        int sockfd;
        struct sockaddr_in destAddr;

};
