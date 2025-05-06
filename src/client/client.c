#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "inetw.h"

#include "chat_protocol.h"

#include "client/tui.h"

#define CLIENT_LEAVE 2

#define WELCOME_MESSAGE "╔══════════════════════════════════\n║\tWelcome to the chat!\n╠══════════════════════════════════\n\n"
#define LEAVE_MESSAGE "╠══════════════════════════════════\n║\t\tBye!\n╚══════════════════════════════════\n"
#define BAR "╠══════════════════════════════════\n"


struct args {
    int *flags_pointer;
    int socket_fd;
    char *username;
    char *buffer;
};

int parse_args(int argc, char *argv[])
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
    struct args *args = arg;
    int *pflg = args->flags_pointer, sfd = args->socket_fd;
    char *buff = args->buffer;
    struct chat_pdu data;

    data.type = CHAT_INFO;
    strcpy(data.username, args->username);
    strcpy(data.content, "/enter");
    send(sfd, &data, sizeof(data), 0);

    data.type = CHAT_MESSAGE;
    while (1) {
        tui_read(buff, TEXT_SIZE);
        strcpy(data.content, buff);

        if (strcmp(buff, "/leave") == 0)
            break;

        send(sfd, &data, sizeof(data), 0);

        printf("\e[A\e[2K\e[8C");
    }

    data.type = CHAT_INFO;
    send(sfd, &data, sizeof(data), 0);

    printf("\e[A\e[2K");

    (*pflg) |= CLIENT_LEAVE;

    return NULL;
}

void *handle_output(void *arg)
{
    struct args *args = arg;
    int *pflg = args->flags_pointer, sfd = args->socket_fd;
    char *buff = args->buffer, special_buff[256];
    struct chat_pdu data;

    recv(sfd, special_buff, sizeof(WELCOME_MESSAGE), 0);
    printf("%s", special_buff);

    printf("\e[8C");

    while (1) {
        if ((*pflg) & CLIENT_LEAVE)
            break;

        recv(sfd, &data, sizeof(data), 0);

        printf("\e[s\e[2K\e[A\e[2K\r");

        if (data.type == CHAT_MESSAGE) {
            printf("║ %s: %s\n", data.username, data.content);
        } else {
            if (data.content[1] == 'e')
                printf("║ \t\033[3m%s connected...\033[0m\n", data.username);
            else
                printf("║ \t\033[3m%s disconnected...\033[0m\n", data.username);
        }

        printf(BAR);
        printf("║ type: %s\e[u\e[B", buff);
        fflush(stdout);
    }

    printf("\e[2K\e[A");

    recv(sfd, special_buff, sizeof(LEAVE_MESSAGE), 0);
    printf("%s", special_buff);

    return NULL;
}

int main(int argc, char *argv[])
{
    int sfd, flags = 0;
    char tty_buffer[TEXT_SIZE] = {0};
    pthread_t incli, outcli;
    in_port_t serv_port;
    struct sockaddr_in myaddr;
    struct args args;

    if (parse_args(argc, argv))
        return 0;

    if (strlen(argv[1]) >= USERNAME_SIZE) {
        fprintf(stderr, "Incorrect username (31 char max)...\n");
        return 0;
    }

    sscanf(argv[3], "%hu", &serv_port);

    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = inet_addr(argv[2]);
    myaddr.sin_port = htons(serv_port);

    if ((sfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Failed to open file descriptor...\n");
        return 0;
    }

    if (connect(sfd, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
        fprintf(stderr, "Failed to connect to %s:%hu...\n", argv[2], serv_port);
        close(sfd);
        return 0;
    }

    args.flags_pointer = &flags;
    args.socket_fd = sfd;
    args.username = argv[1];
    args.buffer = tty_buffer;

    pthread_create(&incli, NULL, handle_input, &args);
    pthread_create(&outcli, NULL, handle_output, &args);

    pthread_join(incli, NULL);
    pthread_join(outcli, NULL);

    close(sfd);

    return 0;
}

