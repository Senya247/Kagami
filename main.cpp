#include "include/client/client.hpp"
#include "include/dev.hpp"
#include "include/net.hpp"
#include "include/server/server.hpp"
#include <iostream>
#include <string>
#include <vector>

int run_server();
int run_client();

int main(int argc, char **argv) {
    if (argc == 1)
        run_server();
    else
        run_client();
}

int run_server() {
    const std::vector<std::string> paths = {"/dev/input/event3",
                                            "/dev/input/event15"};

    Kagami::Server server(34924);
    server.init_devices(paths);

    server.socket_init();
    server.socket_bind();
    server.socket_listen(5);

    int accepting = 1;
    server.accept_clients(&accepting); /* start thread to accept clients */

    std::vector<Kagami::Device *> devs;
    struct Kagami::r_input_event inp_ev;
    do {
        devs = server.poll_devices();
        for (const auto device : devs) {
            device->event_read(&inp_ev);
            server.event_write(&inp_ev);
        }
    } while (devs.size() && accepting);

    return 0;
}

int run_client() {
    Kagami::Client client(34924);

    client.socket_init();
    client.socket_connect("192.168.1.48");
    std::cout << "reached" << std::endl;

    struct Kagami::hint_header hint;
    while (client.hint_receive(&hint)) {
        client.hint_parse(&hint);
    }

    return 0;
}
