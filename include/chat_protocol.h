#ifndef CHAT_PROTOCOL_H
#define CHAT_PROTOCOL__H

#define USERNAME_SIZE 20
#define TEXT_SIZE 44

/* Normal Chat Message */
#define CHAT_MESSAGE 0

/* Chat Information */
#define CHAT_INFO 1

struct chat_pdu {
    int type;
    char username[USERNAME_SIZE];
    char content[TEXT_SIZE];
};

#endif	/* chat_protocol.h */
