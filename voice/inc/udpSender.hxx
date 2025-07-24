#include <arpa/inet.h>
#include <string>
#include <vector>
class udpSender {

    public:
        udpSender(std::string, const uint16_t);
        ~udpSender();

        void init();
        void send(const std::vector<uint8_t> &);

    private:
        int sockfd;
        struct sockaddr_in destAddr;

};
