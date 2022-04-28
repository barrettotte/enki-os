#include "enki.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main(int argc, char** argv) {

    // printf("Hello from userland\n");
    //
    // for (int i = 0; i < argc; i++) {
    //     printf("%s\n", argv[i]);
    // }

    while(1) {
        enki_print(argv[0]);
    }

    return 0;
}
