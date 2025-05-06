#include <string.h>
#include <stdarg.h>

#include "inetw.h"

#include "fd_array.h"


int fdarr_ctl(int *fds, int n, int op, ...)
{
    int i;
    int fd;
    size_t size;
    void *buff;
    va_list args;

    va_start(args, op);

    switch (op) {
        case FDARR_SETUP:
            memset(fds, -1, n * sizeof(*fds));
            break;
        case FDARR_CLEANUP:
            memset(fds, 0, n * sizeof(*fds));
            break;
        case FDARR_ADD:
            if (fds[n - 1] == -1)
                return -1;

            fd = va_arg(args, int);
            fds[n++] = fd;

            for (i = 0; i < n; ++i)
                if (fds[i] == -1) {
                    fds[i] = fd;

                    break;
                }

            break;
        case FDARR_RMV:
            if (fds[0] == -1)
                return -1;

            fd = va_arg(args, int);

            for (i = 0; i < n; ++i) {
                if (fds[i] == fd) {
                    for (; i < n - 1 && fds[i] != -1; ++i)
                        fds[i] = fds[i + 1];

                    fds[n - 1] = -1;

                    break;
                }
            }

            break;
        case FDARR_SENDALL:
            buff = va_arg(args, void *);
            size = va_arg(args, size_t);
            
            for (i = 0; i < n; ++i)
                send(fds[i], buff, n, 0);

            break;
    }

    va_end(args);

    return 0;
}
