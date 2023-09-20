#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>

#include "serverside.h"

/*--------PRIVATE--------*/

#define _SFDARR_RMV 0
#define _SFDARR_ADD 1
#define _SFDARR_SENDALL 2

void _sfdarr_add(int *sfdarr, int *sfdnr, int fd)
{
    int nr = *sfdnr;

    if (nr < 0 || _SOCKETS_LIMIT <= nr)
        return;

    sfdarr[nr] = fd;

    *sfdnr = ++nr;
}

void _sfdarr_rmv(int *sfdarr, int *sfdnr, int fd)
{
    int i, nr = *sfdnr;

    if (nr < 0 || _SOCKETS_LIMIT <= nr)
        return;

    for (i = 0; i < nr; ++i)
        if (sfdarr[i] == fd) {
            *sfdnr = --nr;

            for (; i < nr; ++i)
                sfdarr[i] = sfdarr[i + 1];

            sfdarr[nr] = -1;

            break;
        }
}

void _sfdarr_send(int *sfdarr, int sfdnr, struct chatpkg *data)
{
    int i;

    for (i = 0; i < sfdnr; ++i)
        send(sfdarr[i], data, sizeof(*data), 0);
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
    memset(fdg->socket_fd_array, -1, sizeof(fdg->socket_fd_array));

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

    // printf("astept\n");

    if ((ev_nr = epoll_wait(epfd, ev_arr, _SOCKETS_LIMIT, timeout)) == -1) {
        fprintf(stderr, "Epoll failed...");
        return -1;
    }

    for (i = 0; i < ev_nr; ++i) {
        fd = ev_arr[i].data.fd;

        if (fd == lsfd) {
            new_fd = wrp_accept(lsfd, NULL, log);
            ev.events = EPOLLIN, ev.data.fd = fd;
            epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
            _sfdarr_add(sfdarr, &sfd_nr, fd);

            // recv(new_fd, &data, sizeof(data), 0);

            // _sfdarr_send(sfdarr, sfd_nr, &data);

            printf("%s connected...\n", data.name);
            fprintf(log, "%s connected...\n", data.name);
        } else {
            recv(fd, &data, sizeof(data), 0);
            _sfdarr_send(sfdarr, sfd_nr, &data);

            if (strcmp(data.content, "/leave")) {
                printf("[]%s: %s", data.name, data.content);
                fprintf(msglog, "[]%s: %s", data.name, data.content);
            } else {
                // send(new_fd, _LEAVE_MESS, sizeof(_LEAVE_MESS), 0);

                _sfdarr_rmv(sfdarr, &sfd_nr, fd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
                shutdown(fd, SHUT_RDWR);
                close(fd);

                printf("%s disconnected...\n", data.name);
                fprintf(log, "%s disconnected...\n", data.name);
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

    memset(fdg->socket_fd_array, 0, sizeof(fdg->socket_fd_array));

    epoll_ctl(lsfd, EPOLL_CTL_DEL, lsfd, NULL);
    shutdown(lsfd, SHUT_RDWR);
    close(lsfd);

    close(epfd);
}
