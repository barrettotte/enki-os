#ifndef ENKI_STDLIB_H
#define ENKI_STDLIB_H

#include <stddef.h>

// allocate heap memory
void* malloc(size_t size);

// free allocated heap memory
void free(void* ptr);

// convert integer to string
char* itoa(int i);

#endif