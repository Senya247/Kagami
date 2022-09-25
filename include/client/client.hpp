#pragma once

#include "../../include/dev.hpp"
#include <linux/input.h>
#include <string>
#include <vector>

namespace Kagami {

class Client {
  private:
    int _socket_fd;
    int _port;

    std::vector<Device> _devices; /* uinput devices */

  public:
    Client(int port = 34567) : _port(port){};

    int sockfd() { return _socket_fd; };
    int port() { return _port; };

    int socket_init();
    int socket_connect(std::string ip_addr);

    int hint_receive(struct hint_header *hint);
    int hint_parse(struct hint_header *hint);

    int event_receive(struct r_input_event *event);
    int event_run(const struct r_input_event *event);

    int device_receive(struct Device::dev_info *dev_info);
};

} // namespace Kagami
