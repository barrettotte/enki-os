#ifndef ENKI_H
#define ENKI_H

#include <stdbool.h>
#include <stddef.h>

// print message to terminal
void print(const char* msg);

// get key pressed
int enki_getkey();

// print character to terminal
void enki_putchar(char c);

// allocate heap memory
void* enki_malloc(size_t size);

// free allocated heap memory
void enki_free(void* ptr);

// block until key is pressed and return it
int enki_getkey_block();

// read line of input from terminal
void enki_tty_readline(char* out, int max, bool show_typing);

#endif