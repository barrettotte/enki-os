#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>

#include "../../include/kernel/config.h"

#define HEAP_BLOCK_TABLE_ENTRY_FREE  0x00
#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01

#define HEAP_BLOCK_HAS_NEXT 0b10000000
#define HEAP_BLOCK_IS_FIRST 0b01000000

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

struct heap_table {
    HEAP_BLOCK_TABLE_ENTRY* entries;
    size_t length;
};

struct heap {
    struct heap_table* table;
    void* start_addr;
};

// setup a new heap
int heap_create(struct heap* heap, void* start_addr, void* end_addr, struct heap_table* table);

// allocate heap memory
void* heap_malloc(struct heap* heap, size_t size);

// free heap memory
void heap_free(struct heap* heap, void* addr);

#endif