#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

/* https://stackoverflow.com/a/8488201/13261485 */
#define __FILENAME__                                                           \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

void error_exit(int err_num) {
    fprintf(stderr, "Error in file %s in function %s at line %d:\n%s\n",
            __FILENAME__, __PRETTY_FUNCTION__, __LINE__, strerror(err_num));
    exit(EXIT_FAILURE);
};

int socket_create(int port) {
    int sockfd, result, flag = 1;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        error_exit(errno);

    result = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
    if (result < 0)
        error_exit(errno);

    /* https://stackoverflow.com/a/17843292/13261485 */
    result = setsockopt(sockfd,       /* socket affected */
                        IPPROTO_TCP,  /* set option at TCP level */
                        TCP_NODELAY,  /* name of option */
                        &flag,        /* the cast is historical cruft */
                        sizeof(int)); /* length of option value */
    if (result < 0)
        error_exit(errno);
    return sockfd;
}

void socket_bind(int sockfd, int port) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)))
        error_exit(errno);
}

void socket_listen(int sockfd, int backlog) {
    if (listen(sockfd, backlog) != 0)
        error_exit(errno);
}
