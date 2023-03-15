#include "../include/net.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>

namespace Kagami {

namespace Net {
int addr_create(std::string ip, int port, struct sockaddr_in *address) {
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    inet_aton(ip.c_str(), &(addr.sin_addr));

    std::memcpy(address, &addr, sizeof(*address));
    return 0;
}

size_t write_all(int s, void *buf, size_t len) {
    size_t total = 0;       // how many bytes we’ve sent
    size_t bytesleft = len; // how many we have left to send
    size_t n;
    while (total < len) {
        n = write(s, (char *)buf + total, bytesleft);
        if (n == -1) {
            break;
        }
        total += n;
        bytesleft -= n;
    }
    return n == -1 ? -1
                   : total; // return -1 on failure, bytes written on success
}

size_t read_all(int s, void *buf, size_t len) {
    size_t total = 0;       // how many bytes we've sent
    size_t bytesleft = len; // how many we have left to send
    size_t n;

    while (total < len) {
        n = read(s, (char *)buf + total, bytesleft);
        if (n == -1 || n == 0) {
            break;
        }
        total += n;
        bytesleft -= n;
    }

    return n == -1 ? -1 : total; // return -1 onm failure, bytes read on success
}

} // namespace Net

} // namespace Kagami
