#ifndef ENKI_MEMORY_H
#define ENKI_MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// fill block of memory with value
void* memset(void* ptr, int c, size_t size);

// compare two blocks of memory
int memcmp(void* ptr1, void* ptr2, int size);

// copy memory from one location to another
void* memcpy(void *dest, void* src, int size);

#ifdef __cplusplus
}
#endif

#endif