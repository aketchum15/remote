#include <arpa/inet.h>
#include <string>
#include <vector>
class udpSender {

    public:
        udpSender(std::string, const uint16_t);
        ~udpSender();

        void init();
        void send(const std::vector<int16_t> &);

    private:
        int sockfd;
        struct sockaddr_in destAddr;

};
