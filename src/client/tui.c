#include "client/tui.h"


static void _buff_shl(char *buff, int len)
{
    int i;

    for (i = 0; i < len; ++i) {
        *buff = *(buff + 1);
        buff++;
    }
}

static void _buff_shr(char *buff, int len)
{
    int i;

    buff += len;

    for (i = 0; i < len; ++i) {
        *buff = *(buff - 1);
        buff--;
    }
}

int tui_read(char *buff, int size)
{
    int pos = 0, len = 0;
    char ch = 0;
    char *iter = buff;
    struct termios term;

    if (size <= 1)
        return 0;

    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    *iter = 0;

    while (1) {
        ch = fgetc(stdin);

        if (ch == '\n' || ch == EOF) {
            break;
        } else if (ch == '\e' && fgetc(stdin) == '[') {
            ch = fgetc(stdin);

            if (ch == 'D') {
                if (pos > 0) {
                    iter--;
                    pos--;

                    printf("\e[D");
                }
            } else if (ch == 'C') {
                if (pos < len) {
                    iter++;
                    pos++;

                    printf("\e[C");
                }
            }
        } else if (ch == 127) {
            if (pos > 0 && len) {
                iter--;
                pos--;

                _buff_shl(iter, len - pos);

                len--;
                buff[len] = 0;

                printf("\e[D\e[0K%s", iter);

                if (pos < len)
                    printf("\e[%dD", len - pos);
            }
        } else if (ch != '\t') {
            if (len < size) {
                _buff_shr(iter, len - pos);

                *iter = ch;

                iter++;
                pos++;

                len++;
                buff[len] = 0;

                printf("\e[0K%c%s", ch, iter);

                if (pos < len)
                    printf("\e[%dD", len - pos);
            }
        }
    }

    printf("\n");

    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    return len;
}
