#include <drivers/tty.h>
#include <kernel/panic.h>

void panic(const char* msg) {
    tty_writestr(msg);
    for(;;);
}