#include "enki.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char** argv) {
    print("Hello from userland\n");
    
    print(itoa(123));
    print("\n");
    putchar('X');

    printf("\nThe year is %i\n", 2022);

    void* mem = malloc(512);
    if (!mem) {
        print("Failed to malloc()\n");
    }
    free(mem);

    char buf[1024];
    enki_tty_readline(buf, sizeof(buf), true);
    print(buf);

    while(1) {}

    return 0;
}