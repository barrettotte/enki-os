#include <enki.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

int cmd_echo(char* cmd) {
    printf("%s\n", cmd+5);  // skip over "echo "
    return 0;
}
