#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macrodef.h"

int main(void)
{
    printf(_WELCOME_MESS);
    printf("║\n");
    printf("║\n");
    printf("║\n");
    printf("╠═════════════════════════════════════════\n");
    printf("║\n");
    printf(_LEAVE_MESS);

    return 0;
}
