#ifndef PROTDEF_H
#define PROTDEF_H

#include "macrodef.h"

/* Normal Message */
#define _CHAT_MESSAGE 0

/* Information Message */
#define _CHAT_INFO_MESSAGE 1

struct chatpkg {
    int type;
    char username[_BUFF_USERNAMESIZE];
    char content[_BUFF_TEXTSIZE];
};

#endif	/* protdef.h */
