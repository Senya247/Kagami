#include "../../include/common/err.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <linux/input.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

int socket_create(int port) {
    int sockfd, result, flag = 1;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        error_exit(errno);

    result = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
    if (result < 0)
        error_exit(errno);

    /* https://stackoverflow.com/a/17843292/13261485
     * Disable Nagle's algorithm */
    result = setsockopt(sockfd,       /* socket affected */
                        IPPROTO_TCP,  /* set option at TCP level */
                        TCP_NODELAY,  /* name of option */
                        &flag,        /* the cast is historical cruft */
                        sizeof(int)); /* length of option value */
    if (result < 0)
        error_exit(errno);
    return sockfd;
}

int addr_create(const char *ip, int port, struct sockaddr_in *address) {
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    inet_aton(ip, &(addr.sin_addr));

    memcpy(address, &addr, sizeof(*address));
    return 0;
}

/* yes i know this won't make the code much shorter */
int socket_connect(int sockfd, struct sockaddr_in *addr) {
    int err;
    err = connect(sockfd, (struct sockaddr *)addr, sizeof(*addr));

    return err;
}
