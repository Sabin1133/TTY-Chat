#ifndef SERVERSIDE_H
#define SERVERSIDE_H

#include "../depen.h"
#include "../macrodef.h"
#include "../protdef.h"
#include "apiwrap.h"

#include "sockfdarr.h"


struct fdgroup {
    /* Used to select active fd 
       Contains all fd (listening and the ones in array) */
    int epoll_fd;

    /* Used to accept new connections, create new fd */
    int listening_socket_fd;

    /* fds of clients */
    int socket_fd_array[_SOCKETS_LIMIT];

    /* the epoll will contain all fds meaning the listening one and all
     * the other ones from the array
     */
};

int fdgroup_setup(struct fdgroup *fdg, in_port_t port, FILE *log);
int fdgroup_work(struct fdgroup *fdg, int timeout, FILE *log, FILE *msglog);
void fdgroup_cleanup(struct fdgroup *fdg, FILE *log);

#endif	/* serverside.h */
