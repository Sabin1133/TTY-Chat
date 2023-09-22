#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>
#include "inetw.h"

#include "sockfdarr.h"

int sfdarr_ctl(int *_sfdarr, int _op, ...)
{
    static unsigned int n = 0, limit = 0;

    unsigned int i;
    int fd;
    size_t sz;
    void *ptr;
    va_list args;

    va_start(args, _op);

    switch (_op) {
        case SFDARR_CLEANUP:
            memset(_sfdarr, 0, limit * sizeof(*_sfdarr));
            n = 0;
            limit = 0;
            break;
        case SFDARR_SETUP:
            n = 0;
            limit = va_arg(args, int);
            memset(_sfdarr, -1, limit * sizeof(*_sfdarr));
            break;
        case SFDARR_RMV:
            if (n == 0)
                return -1;

            fd = va_arg(args, int);

            for (i = 0; i < n; ++i) {
                if (_sfdarr[i] == fd) {
                    n--;

                    for (; i < n; ++i)
                        _sfdarr[i] = _sfdarr[i + 1];

                    _sfdarr[n] = -1;

                    break;
                }
            }
            break;
        case SFDARR_ADD:
            if (n == limit)
                return -1;

            fd = va_arg(args, int);
            _sfdarr[n++] = fd;

            break;
        case SFDARR_SENDALL:
            ptr = va_arg(args, void *);
            sz = va_arg(args, size_t);
            
            for (i = 0; i < n; ++i)
                send(_sfdarr[i], ptr, sz, 0);
            // printf("ptr - %s, sz - %ld\n", (char *)ptr, sz);

            break;
    }

    // printf("n - %u, limit - %u\n", n, limit);

    va_end(args);

    return 0;
}
