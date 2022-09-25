#include "../include/net.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>

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
} // namespace Net

} // namespace Kagami
