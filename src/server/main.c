#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
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

int socket_create(int port);
void socket_bind(int sockfd, int port);
void socket_listen(int sockfd, int backlog);

int main(void) {
    int server_sockfd, client_sockfd;
    struct sockaddr_in client_addr;

    server_sockfd = socket_create(PORT);
    socket_bind(server_sockfd, PORT);
    socket_listen(server_sockfd, 3);

    unsigned int len = sizeof(client_addr);
    while ((client_sockfd =
                accept(server_sockfd, (struct sockaddr *)&client_addr, &len))) {
        fprintf(stdout, "Accepted connection");
    }
}
