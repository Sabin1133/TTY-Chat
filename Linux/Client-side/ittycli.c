#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ittycli.h"

void _chbuff_shl(char *buff, int limit)
{
    int i;

    for (i = 0; i < limit; ++i) {
        *buff = *(buff + 1);
        buff++;
    }
}

void _chbuff_shr(char *buff, int limit)
{
    int i;

    buff += limit;

    for (i = 0; i < limit; ++i) {
        *buff = *(buff - 1);
        buff--;
    }
}

int itty_gets(char *buff, int size)
{
    int i;
    int pos = 0, qnt = 0;
    char ch = 0, *iter = buff;
    struct termios term;

    if ((--size) < 1)
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
                if (pos < qnt) {
                    iter++;
                    pos++;

                    printf("\e[C");
                }
            }
        } else if (ch == 127) {
            if (pos > 0 && qnt) {
                iter--;
                pos--;
                _chbuff_shl(iter, qnt - pos);
                qnt--;
                buff[qnt] = 0;

                printf("\e[D\e[0K%s", iter);
                if (pos < qnt)
                    printf("\e[%dD", qnt - pos);
            }
        } else if (ch != '\t') {
            if (qnt < size) {
                _chbuff_shr(iter, qnt - pos);
                *iter = ch;
                iter++;
                pos++;
                qnt++;
                buff[qnt] = 0;

                printf("\e[0K%c%s", ch, iter);
                if (pos < qnt)
                    printf("\e[%dD", qnt - pos);
            }
        }
    }

    printf("\n");

    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    return qnt;
}
