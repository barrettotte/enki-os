#ifndef ENKI_TTY_H
#define ENKI_TTY_H

#include <stddef.h>

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