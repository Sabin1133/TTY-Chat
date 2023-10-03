#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>

#include "serverside.h"

/*--------PRIVATE--------*/

#define _SFDARR_RMV 0
#define _SFDARR_ADD 1
#define _SFDARR_SENDALL 2

void _fdgroup_addfd(struct fdgroup *fdg, int _fd)
{
    struct epoll_event ev = {.events = EPOLLIN, .data.fd = _fd};

    epoll_ctl(fdg->epoll_fd, EPOLL_CTL_ADD, _fd, &ev);
    sfdarr_ctl(fdg->socket_fd_array, SFDARR_ADD, _fd);
}

void _fdgroup_rmvfd(struct fdgroup *fdg, int _fd)
{
    sfdarr_ctl(fdg->socket_fd_array, SFDARR_RMV, _fd);
    epoll_ctl(fdg->epoll_fd, EPOLL_CTL_DEL, _fd, NULL);
}

/*--------PUBLIC--------*/

int fdgroup_setup(struct fdgroup *fdg, in_port_t port, FILE *log)
{
    int i;
    int epfd, lsfd;
    struct sockaddr_in addr;
    struct epoll_event ev;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    epfd = wrp_epoll_create(8, log);
    if (epfd == -1)
        return -1;

    lsfd = wrp_socket(PF_INET, SOCK_STREAM, 0, log);
    if (lsfd == -1) {
        close(epfd);
        return -1;
    }

    if (wrp_bind_listen(lsfd, (struct sockaddr *)&addr, log) == -1) {
        close(epfd);
        close(lsfd);
        return -1;
    }

    ev.events = EPOLLIN, ev.data.fd = lsfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lsfd, &ev);

    fdg->epoll_fd = epfd;
    fdg->listening_socket_fd = lsfd;
    sfdarr_ctl(fdg->socket_fd_array, SFDARR_SETUP, _SOCKETS_LIMIT);

    return 0;
}

int fdgroup_work(struct fdgroup *fdg, int timeout, FILE *log, FILE *msglog)
{
    int epfd = fdg->epoll_fd;
    int lsfd = fdg->listening_socket_fd, *sfdarr = fdg->socket_fd_array;

    int i, ev_nr, sfd_nr = 0;
    int fd, new_fd;
    struct epoll_event ev_arr[_SOCKETS_LIMIT], ev;
    struct chatpkg data;

    if ((ev_nr = epoll_wait(epfd, ev_arr, _SOCKETS_LIMIT, timeout)) == -1) {
        fprintf(stderr, "Epoll failed...");
        return -1;
    }

    for (i = 0; i < ev_nr; ++i) {
        fd = ev_arr[i].data.fd;

        if (fd == lsfd) {
            new_fd = wrp_accept(lsfd, NULL, log);
            _fdgroup_addfd(fdg, new_fd);

            recv(new_fd, &data, sizeof(data), 0);

            send(new_fd, _WELCOME_MESS, sizeof(_WELCOME_MESS), 0);
            sfdarr_ctl(sfdarr, SFDARR_SENDALL, (void *)&data, sizeof(data));

            printf("%s connected...\n", data.username);
            fprintf(log, "%s connected...\n", data.username);
        } else {
            recv(fd, &data, sizeof(data), 0);
            sfdarr_ctl(sfdarr, SFDARR_SENDALL, (void *)&data, sizeof(data));

            if (strcmp(data.content, "/leave")) {
                printf("[]%s: %s\n", data.username, data.content);
                fprintf(msglog, "[]%s: %s\n", data.username, data.content);
            } else {
                send(fd, _LEAVE_MESS, sizeof(_LEAVE_MESS), 0);

                _fdgroup_rmvfd(fdg, fd);
                shutdown(fd, SHUT_RDWR);
                close(fd);

                printf("%s disconnected...\n", data.username);
                fprintf(log, "%s disconnected...\n", data.username);
            }
        }
    }

    return ev_nr;
}

void fdgroup_cleanup(struct fdgroup *fdg, FILE *log)
{
    int i;
    int epfd = fdg->epoll_fd, lsfd = fdg->listening_socket_fd;
    int *sfd_arr = fdg->socket_fd_array;

    for (i = 0; i < _SOCKETS_LIMIT; ++i) {
        if (sfd_arr[i] == -1)
            break;

        epoll_ctl(epfd, EPOLL_CTL_DEL, sfd_arr[i], NULL);
        shutdown(sfd_arr[i], SHUT_RDWR);
        close(sfd_arr[i]);
    }

    sfdarr_ctl(sfd_arr, SFDARR_CLEANUP);

    epoll_ctl(epfd, EPOLL_CTL_DEL, lsfd, NULL);
    shutdown(lsfd, SHUT_RDWR);
    close(lsfd);

    close(epfd);
}
