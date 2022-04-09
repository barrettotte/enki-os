#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include <stddef.h>

// initialize kernel heap
void kheap_init();

// allocate heap memory
void* kmalloc(size_t size);

// allocate heap memory and set allocated bytes to zero
void* kzalloc(size_t size);

// free allocated heap memory
void kfree(void* ptr);

#endif