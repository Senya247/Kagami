#include "../../include/common/err.h"
#include "../../include/common/event.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <linux/input.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define PORT 34924

struct sockaddr_in server;

const char *keyboard_path = "/dev/input/event3";

int socket_create(int port);
void socket_bind(int sockfd, int port);
void socket_listen(int sockfd, int backlog);

int main(void) {
    int err;
    struct libevdev *keyboard;

    err = device_new(keyboard_path, &keyboard);
    if (err < 0)
        error_exit(errno);

    int server_sockfd, client_sockfd;
    struct sockaddr_in client_addr;

    server_sockfd = socket_create(PORT);
    socket_bind(server_sockfd, PORT);
    socket_listen(server_sockfd, 3);

    unsigned int len = sizeof(client_addr);
    /* apparently socket options are inherited through accept */
    client_sockfd =
        accept(server_sockfd, (struct sockaddr *)&client_addr, &len);
    fprintf(stdout, "Connected to %s\n", inet_ntoa(client_addr.sin_addr));

    struct input_event inp_ev;
    while (!device_read_event(keyboard, &inp_ev)) {
        printf("Event: %s %s %d\n", libevdev_event_type_get_name(inp_ev.type),
               libevdev_event_code_get_name(inp_ev.type, inp_ev.code),
               inp_ev.value);
        err = write(client_sockfd, &inp_ev, sizeof(inp_ev));
        if (err == -1)
            error_exit(errno);
    }

    libevdev_free(keyboard); /* will we even reach here -_- */
    return 0;
}
