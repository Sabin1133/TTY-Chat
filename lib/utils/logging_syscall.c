#include <sys/epoll.h>

#include "utils/logging_syscall.h"


static void _log_print(FILE *log, char *buff)
{
    printf(buff);
    fprintf(log, buff);
}

static void _log_err_print(FILE *log, char *buff)
{
    fprintf(stderr, buff);
    fprintf(log, buff);
}

int log_epoll_create(FILE *log, int n)
{
    int fd;

    _log_print(log, "Requesting epoll...\n");

    fd = epoll_create(n);

    if (fd  == -1) {
        _log_err_print(log, "Failed to get epoll...");

        return -1;
    }

    _log_print(log, "Request successful...\n");

    return fd;
}

int log_socket(FILE *log, int domain, int type, int protocol)
{
    int fd;

    _log_print(log, "Requesting listening socket...\n");

    fd = socket(domain, type, protocol);

    if (fd == -1) {
        _log_err_print(log, "Failed to get listening socket...\n");

        return -1;
    }
    
    _log_print(log, "Request successful...\n");

    return fd;
}

int log_bind_and_listen(FILE *log, int fd, struct sockaddr *addr, int n)
{
    int rc;

    _log_print(log, "Binding listening socket to address...\n");

    rc = bind(fd, addr, sizeof(*addr)); 

    if (rc == -1) {
        _log_err_print(log, "Failed to bind listening socket to address...\n");

        return -1;
    }

    _log_print(log, "Binding successful...\n");

    _log_print(log, "Setting listening socket on listen...\n");

    rc = listen(fd, n);

    if (rc == -1) {
        _log_err_print(log, "Failed to set listening socket on listen...\n");

        return -1;
    }
    
    _log_print(log, "Setting successful...\n");

    return 0;
}

int log_accept(FILE *log, int fd, struct sockaddr *addr)
{
    int new_fd;

    _log_print(log, "Accept connection on listening socket...\n");

    new_fd = accept(fd, addr, NULL);

    if (new_fd == -1) {
        _log_err_print(log, "Failed to accept connection on listening socket...\n");

        return -1;
    }

    _log_print(log, "Accept successful...\n");

    return new_fd;
}
