#include "include/client/client.hpp"
#include "include/dev.hpp"
#include "include/err.h"
#include "include/net.hpp"
#include "include/server/server.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <libevdev/libevdev.h>

int run_server();
int run_client();

int main(int argc, char **argv) {
    if (argc == 1)
        run_server();
    else
        run_client();
}

int run_server() {
    std::cout << "Running as server" << std::endl;

    const std::vector<std::string> paths = {
        "/dev/input/event3",
        "/dev/input/event5",
    };

    Kagami::Server server(34924);
    server.init_devices(paths);

    server.socket_init();
    server.socket_bind();
    server.socket_listen(5);

    server.socket_accept();

    if (server.poll_init() < 0)
        error_exit(errno);

    std::vector<Kagami::Device *> ready_to_read;
    struct Kagami::r_input_event inp_ev;
    do {
        server.poll_devices(ready_to_read);
        for (const auto device : ready_to_read) {
            device->event_read(&inp_ev);
            printf("Event: %s %s %d\n", libevdev_event_type_get_name(inp_ev.event.type),
                   libevdev_event_code_get_name(inp_ev.event.type, inp_ev.event.code), inp_ev.event.value);
            if (server.event_write(&inp_ev) < 0)
                error_exit(errno);
        }
    } while (server.clients_num());
    puts("end");

    return 0;
}

int run_client() {
    std::cout << "Running as client" << std::endl;

    Kagami::Client client;

    client.socket_init();
    if (client.socket_connect("100.100.173.124") < 0) {
        printf("error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct Kagami::hint_header hint;

    int recvd;
    while ((recvd = client.hint_receive(&hint))) {
        if (recvd != sizeof(hint))
            error_exit(errno);

        client.hint_parse(&hint);
    }

    return 0;
}
