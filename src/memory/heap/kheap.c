#include "../../config.h"
#include "../../kernel.h"
#include "kheap.h"
#include "heap.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kheap_init() {
    int total_entries = ENKI_HEAP_SIZE_BYTES / ENKI_HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*) ENKI_HEAP_TABLE_ADDRESS;
    kernel_heap_table.length = total_entries;

    void* end_addr = (void*) (ENKI_HEAP_ADDRESS + ENKI_HEAP_SIZE_BYTES);
    int status = heap_create(&kernel_heap, (void*) ENKI_HEAP_ADDRESS, end_addr, &kernel_heap_table);
    if (status < 0) {
        print("Failed to create heap\n"); // TODO: panic
    }
}

void* kmalloc(size_t size) {
    return heap_malloc(&kernel_heap, size);
}

void kfree(void* ptr) {
    heap_free(&kernel_heap, ptr);
}
