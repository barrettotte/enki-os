#ifndef ENKI_TTY_H
#define ENKI_TTY_H

#include <stddef.h>

// void tty_init();
// void tty_putchar(char c);
// void tty_write(const char* s, size_t size);
// void tty_writestr(const char* s);

// initialize terminal
void tty_init();

// place char at (x,y) with color
void tty_putchar(int x, int y, char c, char color);

// backspace in terminal
void tty_backspace();

// write char to terminal
void tty_writechar(char c, char color);

// write string to terminal
void tty_writestr(const char *s);

#endif