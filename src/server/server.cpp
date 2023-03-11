#include "../../include/err.h"
#include "../../include/net.hpp"
#include "../../include/server/server.hpp"

#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <iterator>
#include <libevdev/libevdev.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <ostream>
#include <poll.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

namespace Kagami {

int Server::fd() { return _fd; }
int Server::port() { return _port; }

int Server::socket_init() {
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

    _fd = sockfd;
    return 0;
}

int Server::socket_bind() {
    int err;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    err = bind(_fd, (struct sockaddr *)&addr, sizeof(addr));

    return err;
}

int Server::socket_listen(int backlog) { return listen(_fd, backlog); }

int Server::init_devices(const std::vector<std::string> dev_paths) {
    int id = 0;
    for (const auto &path : dev_paths) {
        /* printf("initing %s\n", path.c_str()); */
        Device dev;
        if (dev.init(path, id++) < 0) {
            printf("error: %s\n", strerror(errno));
            return -1;
        }
        _devices.push_back(dev);
    }

    return 0;
}

int Server::socket_accept() {
    int fd;
    struct sockaddr_in addr;
    unsigned int len = sizeof(addr);

    fd = accept(_fd, (struct sockaddr *)&addr, &len);

    Client_ctx client;
    client.init(fd, &addr);

    _clients.push_back(client);
    std::cout << "accepted client" << std::endl;

    /* send client device info */
    write_dev_info(&client);
    puts("written dev info");

    return 0;
}

int Server::write_dev_info(Client_ctx *ctx) {
    struct Device::dev_info info;
    struct hint_header header = {.hint = K_HINTS_DEVNEW};

    for (auto &device : _devices) {
        std::memset(&info, 0, sizeof(info));
        device.dev_info(&info);
        info.id = device.id();

        std::cout << "Writing info for " << device.name() << std::endl;
        write(ctx->fd(), &header, sizeof(header)); /* TODO: this is ugly */
        write(ctx->fd(), &info, sizeof(info));
    }

    return 0;
}

int Server::clients_num() { return _clients.size(); }
int Server::dev_num() { return _devices.size(); }

int Server::poll_init() {
    int err;
    int len = dev_num();

    _pollfds = (struct pollfd *)malloc(sizeof(struct pollfd) * len);

    for (int i = 0; i < len; i++) {
        printf("adding %s(%d) to pollfds\n", _devices[i].name().c_str(),
               _devices[i].fd());
        _pollfds[i].fd = _devices[i].fd();

        _pollfds[i].events = POLLIN;

        if (_pollfds[i].fd == -1) {
            printf("error in pollfd: %s\n", strerror(errno));
            free(_pollfds);
            return -1;
        }
    }

    for (int i = 0; i < len; i++) {
    }

    return 0;
}

/* return pointer to device in which the event occured */
std::vector<Device *> Server::poll_devices() {
    int len, err, num_open_fds;
    std::vector<Device *> ret;

    len = dev_num();
    num_open_fds = dev_num();

    err = poll(_pollfds, len, -1);
    if (err == -1)
        goto error;

    for (int i = 0; i < len; i++) {
        if (_pollfds[i].revents & POLLIN) { /* ready to read */
            /* std::cout << "activity in " << _pollfds[i].fd << std::endl; */
            ret.push_back(&(_devices[i]));
        }
    }
    return ret;

error:
    error_exit(errno);
    free(_pollfds);
    return ret;
}

int Server::accept_clients(int *res) {
    auto accept_clients_thread = [this](int *running) {
        std::cout << "started accepting clients" << std::endl;
        while (!this->socket_accept()) {
        }
        std::cout << "stopped accepting clients" << std::endl;
        *running = 0;
    };
    std::thread t(accept_clients_thread, res);
    t.detach();

    return 0;
}

int Server::event_write(const struct r_input_event *event) {
    int ret;
    for (auto &client : _clients) {
        ret = client.event_write(event);
        if (ret != 0)
            return -1;
    }

    return 0;
}

} // namespace Kagami

namespace Kagami {
int Client_ctx::init(int fd, struct sockaddr_in *addr) {
    _fd = fd;
    _ip = inet_ntoa(addr->sin_addr);
    _addr = *addr;

    return 0;
}

int Client_ctx::fd() { return _fd; }

std::string Client_ctx::ip() { return _ip; }

struct sockaddr_in Client_ctx::addr() {
    return _addr;
}

int Client_ctx::event_write(const struct r_input_event *event) {
    /* TODO: this is ugly */
    struct hint_header header;
    header.hint = K_HINTS_EVNEW;

    int ret;

    ret = write(_fd, &header, sizeof(header));
    if (ret != sizeof(header)) {
        goto error;
    }

    ret = write(_fd, event, sizeof(*event));
    if (ret != sizeof(*event)) {
        goto error;
    }

    return 0;

error:
    return ret;
}

} // namespace Kagami
