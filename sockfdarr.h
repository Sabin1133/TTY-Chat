#ifndef SOCKFDARR_H
#define SOCKFDARR_H

/* For cleaning the array */
#define SFDARR_CLEANUP 0

/* For initializing the array */
#define SFDARR_SETUP 1

/* Array commands */
#define SFDARR_RMV 2
#define SFDARR_ADD 3
#define SFDARR_SENDALL 4

int sfdarr_ctl(int *_sfdarr, int _op, ...);

#endif	/* sockfdarr.h */
