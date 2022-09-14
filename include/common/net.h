#pragma once
#include <netinet/in.h>

int socket_create(int port);

int addr_create(const char *ip, int port, struct sockaddr_in *address);

int socket_connect(int sockfd, struct sockaddr_in *addr);
