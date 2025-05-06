#ifndef SOCKFDARR_H
#define SOCKFDARR_H

/* Set up array */
#define FDARR_SETUP 0

/* Clean up array */
#define FDARR_CLEANUP 1

/* Array actions */
#define FDARR_ADD 2
#define FDARR_RMV 3
#define FDARR_SENDALL 4

int fdarr_ctl(int *fd_array, int n, int op, ...);

#endif	/* fd_array.h */
