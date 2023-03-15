#include "../../include/dev.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/poll.h>
#include <vector>

namespace Kagami {

class Client_ctx {
  private:
    int _fd;
    struct sockaddr_in _addr;

    std::string _ip;

  public:
    int init(int fd, struct sockaddr_in *addr);

    int fd();
    std::string ip();

    struct sockaddr_in addr();

    int event_write(const struct r_input_event *event);
};

class Server {
  private:
    int _fd; /* fd to listen on */
    int _port = 0;

    struct pollfd *_pollfds;

    std::vector<Client_ctx> _clients;

    std::vector<Device> _devices;
  public:
    Server(int port = 34924) : _port(port){};

    int init_devices(const std::vector<std::string> dev_paths);

    int fd();
    int port();

    int clients_num();
    int dev_num();

    int socket_init();
    int socket_bind();
    int socket_listen(int backlog);
    int socket_accept();

    int accept_clients(int *res); /* start thread to accept clients */

    int write_dev_info(Client_ctx *ctx);
    int event_write(
        const struct r_input_event *event); /* send event to every client */

    int poll_init();
    int poll_devices(std::vector<Device *>& device_list);
};

} // namespace Kagami
