#include "../../include/client/client.hpp"
#include "../../include/dev.hpp"
#include "../../include/err.h"
#include "../../include/net.hpp"
#include "libevdev/libevdev-uinput.h"

#include <cstdio>
#include <iostream>
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
    int ret;

    switch (hint->hint) {
    case K_HINTS_EVNEW: {
        struct r_input_event event;

        ret = event_receive(&event);
        if (ret != sizeof(event))
            error_exit(errno);

        if (event_run(&event) < 0)
            error_exit(errno);
        break;
    }

    case K_HINTS_DEVNEW: {
        struct Device::dev_info dev_info;

        ret = device_receive(&dev_info);
        if (ret != sizeof(dev_info))
            error_exit(errno);

        std::cout << "new device " << dev_info.name << std::endl;

        /* int event, code;
        for (event = 0; event < EV_MAX; event++) {
            if (test_bit(event, dev_info.event_info)) {
                printf("Event type: %d\n", event);
                for (code = 0; code < KEY_MAX; code++) {
                    if (test_bit(code, dev_info.code_info[event]))
                        printf("  Code: %d\n", code);
                }
            }
        } */

        Device n_dev;
        if (n_dev.init_uinput(&dev_info) < 0)
            error_exit(errno);
        std::cout << "initialized uinput for id " << dev_info.id << std::endl;

        _devices.push_back(n_dev);
        break;
    }

    default:
        break;
    }
    return 0;
}

int Client::event_receive(struct r_input_event *event) {
    return Net::read_all(_socket_fd, event, sizeof(*event));
}

int Client::device_receive(struct Device::dev_info *dev_info) {
    return Net::read_all(_socket_fd, dev_info, sizeof(*dev_info));
}

int Client::event_run(const struct r_input_event *event) {
    /* I commented this out before, can't remember why, might be somethign to
     * look into */
    /* if (event->event.type == 0x4 && event->event.code == 0x4)
        return 0; */
    for (auto &device : _devices) {
        if (device.id() == event->id) {
            return device.event_run(&(event->event));
        }
    }
    fprintf(stderr, "No device of id %d\n", event->id);
    return -1;
}

} // namespace Kagami
