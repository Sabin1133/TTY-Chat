#ifndef FD_GROUP_H
#define FD_GROUP_H

#include <sys/epoll.h>

#include "utils/logging_syscall.h"

#include "fd_array.h"

#define SOCKETS_LIMIT 32
#define LISTEN_LIMIT 8


struct fd_group {
    /* `epoll` kernel data structure used to multiplex fds */
    int epoll_fd;

    /* socket listening for incoming connections */
    int listening_socket;

    /* sockets of client connections */
    int clients_sockets[SOCKETS_LIMIT];
};

int fdgrp_setup(struct fd_group *fdg, FILE *log, in_addr_t inet_address, in_port_t inet_port);
int fdgrp_run(struct fd_group *fdg, FILE *log, FILE *traffic_log);
void fdgrp_cleanup(struct fd_group *fdg, FILE *log);

#endif	/* fd_group.h */
