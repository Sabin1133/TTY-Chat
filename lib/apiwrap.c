#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apiwrap.h"

int wrp_epoll_create(int n, FILE *log)
{
    int fd;

    printf("Requesting epoll...\n");
    fprintf(log, "Requesting epoll...\n");
    if ((fd = epoll_create(n)) == -1) {
        fprintf(stderr, "Failed to get epoll...");
        fprintf(log, "Failed to get epoll...");
        return -1;
    }
    printf("Request successful...\n");
    fprintf(log, "Request successful...\n");

    return fd;
}

int wrp_socket(int domain, int type, int protocol, FILE *log)
{
    int fd;

    printf("Requesting listening socket...\n");
    fprintf(log, "Requesting listening socket...\n");
    if ((fd = socket(domain, type, protocol)) == -1) {
        fprintf(stderr, "Failed to get listening socket...\n");
        fprintf(log, "Failed to get listening socket...\n");
        return -1;
    }
    printf("Request successful...\n");
    fprintf(log, "Request successful...\n");

    return fd;
}

int wrp_bind_listen(int fd, struct sockaddr *addr, FILE *log)
{
    printf("Requesting to bind address to listening socket...\n");
    fprintf(log, "Requesting to bind address to listening socket...\n");
    if (bind(fd, addr, sizeof(*addr)) == -1) {
        fprintf(stderr, "Failed to bind address to listening socket...\n");
        fprintf(log, "Failed to bind address to listening socket...\n");
        return -1;
    }
    printf("Request successful...\n");
    fprintf(log, "Request successful...\n");

    printf("Requesting listen permission on listening socket...\n");
    fprintf(log, "Requesting listen permission on listening socket...\n");
    if (listen(fd, _LISTEN_LIMIT) == -1) {
        fprintf(stderr, "Failed to listen on listening socket...\n");
        fprintf(log, "Failed to listen on listening socket...\n");
        return -1;
    }
    printf("Request successful...\n");
    fprintf(log, "Request successful...\n");

    return 0;
}

int wrp_accept(int lsfd, struct sockaddr *addr, FILE *log)
{
    int fd;
    printf("Requesting accept on listening socket...\n");
    fprintf(log, "Requesting accept on listening socket...\n");
    if ((fd = accept(lsfd, addr, NULL)) == -1) {
        fprintf(stderr, "Failed to accept on listening socket...\n");
        fprintf(log, "Failed to accept on listening socket...\n");
        return -1;
    }
    printf("Request successful...\n");
    fprintf(log, "Request successful...\n");

    return fd;
}
