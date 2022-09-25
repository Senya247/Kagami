#include "../../include/client/client.hpp"
#include "../../include/dev.hpp"
#include "../../include/net.hpp"
#include "libevdev/libevdev-uinput.h"

#include <linux/input.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

namespace Kagami {

int Client::socket_init() {
    int sockfd, result, flag = 1;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        return sockfd;

    result = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
    if (result < 0)
        return result;

    /* https://stackoverflow.com/a/17843292/13261485
     * Disable Nagle's algorithm */
    result = setsockopt(sockfd,       /* socket affected */
                        IPPROTO_TCP,  /* set option at TCP level */
                        TCP_NODELAY,  /* name of option */
                        &flag,        /* the cast is historical cruft */
                        sizeof(int)); /* length of option value */
    if (result < 0)
        return result;

    _socket_fd = sockfd;
    return 0;
}

int Client::socket_connect(std::string ip) {
    struct sockaddr_in addr;

    Kagami::Net::addr_create(ip, _port, &addr);

    return connect(_socket_fd, (struct sockaddr *)&addr, sizeof(addr));
}

int Client::hint_receive(struct hint_header *hint) {
    return read(_socket_fd, hint, sizeof(*hint));
}

int Client::hint_parse(struct hint_header *hint) {
    switch (hint->hint) {
    case K_HINTS_EVNEW: {
        static struct r_input_event event;
        event_receive(&event);
        event_run(&event);
        break;
    }

    case K_HINTS_DEVNEW: {
        struct Device::dev_info dev_info;
        device_receive(&dev_info);

        Device n_dev;
        n_dev.init_uinput(&dev_info);

        _devices.push_back(n_dev);
        break;
    }

    default:
        break;
    }

    return 0;
}

int Client::event_receive(struct r_input_event *event) {
    return read(_socket_fd, event, sizeof(*event));
}

int Client::device_receive(struct Device::dev_info *dev_info) {
    return read(_socket_fd, dev_info, sizeof(*dev_info));
}

int Client::event_run(const struct r_input_event *event) {
    for (auto &device : _devices) {
        if (device.id() == event->id) {
            return device.event_run(&(event->event));
        }
    }
    return -1;
}

} // namespace Kagami
