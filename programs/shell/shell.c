#include <enki.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shell.h"

// route to command based on user input
static void cmd_handler(char* in) {
    if (!strncmp("echo", in, 4)) {
        cmd_echo(in);
    } else {
        struct cmd_arg* cmd = enki_parse_cmd(in, sizeof(in));
        enki_system_run(cmd->arg);  // otherwise try to run file
    }
}

int main(int argcc, char** argv) {
    enki_print("EnkiOS\n");

    while(1) {
        char buf[1024];
        enki_print("> ");
        enki_tty_readline(buf, sizeof(buf), true);
        enki_print("\n");
        cmd_handler(buf);
    }
    return 0;
}