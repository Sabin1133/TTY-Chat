#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clientside.h"

#define CLIENT_LEAVE 2

struct thd_client_args {
    int *flags_pointer;
    int socket_fd;
    char *username;
    char *buffer;
};

int parse_cli_arg(int argc, char *argv[])
{
    if ((argc != 4) || (argc == 2 && strcmp(argv[1], "--help") == 0)) {
        printf("Usage:\n");
        printf("\t./client \033[1m[username] [ip address] [port]\033[0m\n\n");
        return 1;
    }

    return 0;
}

void *handle_input(void *arg)
{
    struct thd_client_args *args = arg;
    int *pflg = args->flags_pointer, sfd = args->socket_fd;
    char *buff = args->buffer;
    struct chatpkg data;

    data.type = _CHAT_INFO_MESSAGE;
    strncpy(data.name, args->username, _BUFF_NAMESIZE - 1);
    strcpy(data.content, "/connect");
    send(sfd, &data, sizeof(data), 0);

    data.type = _CHAT_MESSAGE;

    while (1) {
        itty_gets(buff, _BUFF_TEXTSIZE);
        strcpy(data.content, buff);

        if (strcmp(buff, "/leave"))
            break;

        send(sfd, &data, sizeof(data), 0);

        printf("\e[A\r");
    }

    data.type = _CHAT_INFO_MESSAGE;
    send(sfd, &data, sizeof(data), 0);

    (*pflg) |= CLIENT_LEAVE;

    return NULL;
}

void *handle_output(void *arg)
{
    struct thd_client_args *args = arg;
    int *pflg = args->flags_pointer, sfd = args->socket_fd;
    char *buff = args->buffer;
    struct chatpkg data;

    while (1) {
        if ((*pflg) & CLIENT_LEAVE)
            break;

        recv(sfd, &data, sizeof(data), 0);

        printf("\e[s\e[2K\e[A\e[2K\r");

        if (data.type = _CHAT_MESSAGE) {
            printf("║%s: %s\n", data.name, data.content);
        } else {
            if (data.content[1] == 'c')
                printf("\t\033[3m%s connected...\033[0m\n", data.name);
            else
                printf("\t\033[3m%s disconnected...\033[0m\n", data.name);
        }

        printf("╠═════════════════════════════════════════\n║");
        printf("%s\e[u\e[B", buff);
        fflush(stdout);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    int sfd, flags = 0;
    char tty_buffer[_BUFF_TEXTSIZE] = {0};
    pthread_t cli, chat;
    in_port_t selected_port;
    struct sockaddr_in myaddr;
    struct thd_client_args args;

    if (parse_cli_arg(argc, argv))
        return 0;

    sscanf(argv[3], "%hu", &selected_port);

    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = inet_addr(argv[2]);
    myaddr.sin_port = htons(selected_port);

    if ((sfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Failed to open file descriptor...\n");
        return 0;
    }

    if (connect(sfd, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
        fprintf(stderr, "Failed to connect to %s:%hu...\n", argv[2], selected_port);
        close(sfd);
        return 0;
    }

    args.flags_pointer = &flags;
    args.socket_fd = sfd;
    args.username = argv[1];
    args.buffer = tty_buffer;

    pthread_create(&cli, NULL, handle_input, &args);
    pthread_create(&chat, NULL, handle_output, &args);

    pthread_join(cli, NULL);
    pthread_join(chat, NULL);

    close(sfd);

    return 0;
}

