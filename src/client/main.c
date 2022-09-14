#include "../../include/common/err.h"
#include "../../include/common/event.h"
#include "../../include/common/net.h"
#include "../../include/common/uinput.h"
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

const char *server_ip = "192.168.1.48";
const char *keyboard_path = "/dev/input/event3"; /* Camelot */

int main(void) {
    struct libevdev_uinput *uidev;
    uinput_create_from_device(keyboard_path, &uidev);

    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket_create(PORT);
    if (sockfd < 0)
        error_exit(errno);
    addr_create(server_ip, PORT, &server_addr);

    socket_connect(sockfd, &server_addr);

    /* start receiving events */

    struct input_event inp_ev;
    while (read(sockfd, &inp_ev, sizeof(inp_ev))) {
        write_uintput_event(uidev, &inp_ev);
    }
}
