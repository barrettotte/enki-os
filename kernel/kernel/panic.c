#include "../include/kernel/panic.h"
#include "../include/kernel/tty.h"

void panic(const char* msg) {
    tty_writestr(msg);
    for(;;);
}