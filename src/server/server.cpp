#include "../../include/server/server.hpp"
#include "../include/err.h"
#include "libevdev/libevdev.h"
#include <arpa/inet.h>
#include <iostream>
#include <iterator>
#include <netinet/in.h>
#include <netinet/tcp.h>
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
        Device dev;
        dev.init(path, id++);
        std::cout << "reached" << std::endl;
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
    std::cout << "Acceped client" << std::endl;

    return 0;
}

int Server::clients_num() { return _clients.size(); }

/* return pointer to device in which the event occured */
std::vector<Device *> Server::poll_devices() {
    int err;
    int len = clients_num();
    std::vector<Device *> ret;

    struct pollfd *pollfds;

    pollfds = (struct pollfd *)malloc(sizeof(struct pollfd) * len);

    for (int i = 0; i < len; i++) {
        pollfds[i].fd = _devices[i].fd();
        pollfds[i].events = POLLIN;

        if (pollfds[i].fd == -1) {
            free(pollfds);
            return ret;
        }
    }

    int num_open_fds = len;
    err = poll(pollfds, len, -1);
    if (err == -1)
        goto error;

    for (int j = 0; j < len; j++) {
        if (pollfds[j].revents & POLLIN) { /* ready to read */
            /* send_event_to_sock(devices[j], sockfd); */
            ret.push_back(&(_devices[j]));
        }
    }

error:
    free(pollfds);
    return ret;
}

int Server::accept_clients(int *res) {
    std::cout << "started accepting clients" << std::endl;
    auto accept_clients_thread = [this](int *running) {
        std::cout << "started accepting clients" << std::endl;
        while (this->socket_accept()) {
        }
        *running = 0;
    };
    std::thread t(accept_clients_thread, res);

    return 0;
}

int Server::event_write(const struct r_input_event *event) {
    for (auto &client : _clients) {
        client.event_write(event);
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
    return write(_fd, event, sizeof(*event));
}

} // namespace Kagami
