#ifndef APIWRAP_H
#define APIWRAP_H

#include "../depen.h"

#define _LISTEN_LIMIT 8

int wrp_epoll_create(int n, FILE *log);

int wrp_socket(int domain, int type, int protocol, FILE *log);
int wrp_bind_listen(int fd, struct sockaddr *addr, FILE *log);
int wrp_accept(int lsfd, struct sockaddr *addr, FILE *log);

#endif	/* sockwrap.h */
