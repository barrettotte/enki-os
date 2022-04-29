#include "enki.h"
#include "shell.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argcc, char** argv) {
    enki_print("EnkiOS\n");

    while(1) {
        char buf[1024];
        enki_print("> ");
    
        enki_tty_readline(buf, sizeof(buf), true);
        enki_print("\n");

        enki_system_run(buf);
        enki_print("\n");
    }
    return 0;
}