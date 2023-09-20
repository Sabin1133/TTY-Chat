#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "serverside.h"

#define CHAT_CLOSE 2
#define WORK_TIME 500

struct thd_serv_args {
    int *flag_pointer;
    struct fdgroup *fdg;
    FILE *activity_log;
    FILE *messages_log;
};

int parse_cli_arg(int argc, char *argv[])
{
    if ((argc != 2) || (argc == 2 && strcmp(argv[1], "--help") == 0)) {
        printf("Usage:\n\t./chatserver \033[1m[port]\033[0m\n\n");
        printf("port - the port the server will be running on\n");
        return 1;
    }

    return 0;
}

void *handle_server(void *arg)
{
    int *pflg = arg;
    char buff[16] = {0};

    while (strcmp(buff, "closetower\n"))
        fgets(buff, sizeof(buff), stdin);

    (*pflg) |= CHAT_CLOSE;

    return NULL;
}

void *handle_work(void *arg)
{
    struct thd_serv_args *args = arg;
    int *pflg = args->flag_pointer;
    struct fdgroup *fdg = args->fdg;
    FILE *log = args->activity_log, *msglog = args->messages_log;

    fprintf(log, "Starting service...\n");
    printf("Starting service...\n");

    while (1) {
        if (*pflg & CHAT_CLOSE)
            break;

        fdgroup_work(fdg, WORK_TIME, log, msglog);
    }

    fprintf(log, "Closing service...\n");
    printf("Closing service...\n");

    return NULL;
}

int main(int argc, char *argv[])
{
    int flags = 0;
    in_port_t selected_port;
    pthread_t cli, chat;
    struct fdgroup fdgr;
    struct thd_serv_args args;
    FILE *act_log, *mess_log;

    if (parse_cli_arg(argc, argv))
        return 0;

    printf("Starting server...\n");

    sscanf(argv[1], "%hd", &selected_port);

    if ((act_log = fopen("activity_log.txt", "w")) == NULL) {
        printf("Closing server...\n");
        return 0;
    }

    if ((mess_log = fopen("message_log.txt", "w")) == NULL) {
        fclose(act_log);
        printf("Closing server...\n");
        return 0;
    }

    if (fdgroup_setup(&fdgr, selected_port, act_log)) {
        fclose(mess_log);
        fclose(act_log);
        printf("Closing server...\n");
        return 0;
    }

    args.flag_pointer = &flags;
    args.fdg = &fdgr;
    args.activity_log = act_log;
    args.messages_log = act_log;

    pthread_create(&cli, NULL, handle_server, &flags);
    pthread_create(&chat, NULL, handle_work, &args);

    pthread_join(cli, NULL);
    pthread_join(chat, NULL);

    fdgroup_cleanup(&fdgr, act_log);
    fclose(mess_log);
    fclose(act_log);

    printf("Closing server...\n");

    return 0;
}
