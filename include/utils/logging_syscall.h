#ifndef LOGGING_SYSCALL_H
#define LOGGING_SYSCALL_H

#include <stdio.h>

#include <netinet/in.h>


int log_epoll_create(FILE *log, int n);

int log_socket(FILE *log, int domain, int type, int protocol);
int log_bind_and_listen(FILE *log, int fd, struct sockaddr *addr, int n);
int log_accept(FILE *log, int fd, struct sockaddr *addr);

#endif	/* logging_syscall.h */
