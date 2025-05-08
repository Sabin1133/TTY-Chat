#include <unistd.h>
#include <sys/epoll.h>

#include <string.h>

#include "server/fd_group.h"

#include "utils/logging_syscall.h"

#include "chat_protocol.h"

#include "fd_array.h"

#define WELCOME_MESSAGE "╔══════════════════════════════════\n║\tWelcome to the chat!\n╠══════════════════════════════════\n\n"
#define LEAVE_MESSAGE "╠══════════════════════════════════\n║\t\tBye!\n╚══════════════════════════════════\n"
#define BAR "╠══════════════════════════════════\n"


static void _fdgrp_add_fd(struct fd_group *fdg, int fd)
{
    struct epoll_event ev = {.events = EPOLLIN, .data.fd = fd};

    fdarr_ctl(fdg->clients_sockets, fdg->sockets_n, FDARR_ADD, fd);
    epoll_ctl(fdg->epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

static void _fdgrp_rmv_fd(struct fd_group *fdg, int fd)
{
    epoll_ctl(fdg->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    fdarr_ctl(fdg->clients_sockets, fdg->sockets_n, FDARR_RMV, fd);
}

int fdgrp_setup(struct fd_group *fdg, FILE *log, in_addr_t inet_address, in_port_t inet_port)
{
    int rc;
    int epoll_fd, lis_socket;
    struct sockaddr_in addr;
    struct epoll_event ev;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(inet_address);
    addr.sin_port = htons(inet_port);

    epoll_fd = log_epoll_create(log, 8);

    if (epoll_fd == -1)
        return -1;

    lis_socket = log_socket(log, PF_INET, SOCK_STREAM, 0);

    if (lis_socket == -1) {
        close(epoll_fd);

        return -1;
    }

    rc = log_bind_and_listen(log, lis_socket, (struct sockaddr *)&addr, LISTEN_LIMIT);

    if (rc == -1) {
        close(epoll_fd);
        close(lis_socket);

        return -1;
    }

    ev.events = EPOLLIN, ev.data.fd = lis_socket;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, lis_socket, &ev);

    fdg->epoll_fd = epoll_fd;
    fdg->listening_socket = lis_socket;
    fdg->sockets_n = 0;
    fdarr_ctl(fdg->clients_sockets, SOCKETS_LIMIT, FDARR_SETUP);

    return 0;
}

int fdgrp_run(struct fd_group *fdg, FILE *log, FILE *traffic_log)
{
    int i;
    int fd, new_fd;
    int ep_fd = fdg->epoll_fd;
    int lis_sock = fdg->listening_socket;
    int events_n = 0;
    int *sockets = fdg->clients_sockets;
    struct epoll_event events[SOCKETS_LIMIT];
    struct chat_pdu data;


    events_n = epoll_wait(ep_fd, events, SOCKETS_LIMIT, -1);

    if (events_n == -1) {
        fprintf(stderr, "Epoll waiting failed...");

        return -1;
    }

    for (i = 0; i < events_n; ++i) {
        fd = events[i].data.fd;

        if (fd == lis_sock) {
            new_fd = log_accept(log, lis_sock, NULL);

            _fdgrp_add_fd(fdg, new_fd);

            recv(new_fd, &data, sizeof(data), 0);

            send(new_fd, WELCOME_MESSAGE, sizeof(WELCOME_MESSAGE), 0);
            fdarr_ctl(sockets, SOCKETS_LIMIT, FDARR_SENDALL, (void *)&data, sizeof(data));

            printf("%s connected...\n", data.username);
            fprintf(log, "%s connected...\n", data.username);
        } else {
            recv(fd, &data, sizeof(data), 0);

            fdarr_ctl(sockets, SOCKETS_LIMIT, FDARR_SENDALL, (void *)&data, sizeof(data));

            if (strcmp(data.content, "/leave")) {
                printf("[]%s: %s\n", data.username, data.content);
                fprintf(traffic_log, "[]%s: %s\n", data.username, data.content);
            } else {
                send(fd, LEAVE_MESSAGE, sizeof(LEAVE_MESSAGE), 0);

                _fdgrp_rmv_fd(fdg, fd);
                
                shutdown(fd, SHUT_RDWR);
                close(fd);

                printf("%s disconnected...\n", data.username);
                fprintf(log, "%s disconnected...\n", data.username);
            }
        }
    }

    return events_n;
}

void fdgrp_cleanup(struct fd_group *fdg, FILE *log)
{
    int i;
    int ep_fd = fdg->epoll_fd;
    int lis_sock = fdg->listening_socket;
    int *sockets = fdg->clients_sockets;

    for (i = 0; i < SOCKETS_LIMIT; ++i) {
        if (sockets[i] == -1)
            break;

        epoll_ctl(ep_fd, EPOLL_CTL_DEL, sockets[i], NULL);

        shutdown(sockets[i], SHUT_RDWR);
        close(sockets[i]);
    }

    fdarr_ctl(sockets, SOCKETS_LIMIT, FDARR_CLEANUP);

    epoll_ctl(ep_fd, EPOLL_CTL_DEL, lis_sock, NULL);

    shutdown(lis_sock, SHUT_RDWR);
    close(lis_sock);

    close(ep_fd);
}
