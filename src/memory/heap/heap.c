#include "../../kernel.h"
#include "../../status.h"
#include "../memory.h"
#include "heap.h"

#include <stdbool.h>

// validate table was setup correctly
static int heap_validate_table(void* start_addr, void* end_addr, struct heap_table* table) {
    int status = 0;
    size_t table_size = (size_t)(end_addr - start_addr);
    size_t total_blocks = table_size / ENKI_HEAP_BLOCK_SIZE;

    if (table->length != total_blocks) {
        status = -EINVARG;
        goto out;
    }
out:
    return status;
}

// validate heap is properly aligned
static bool heap_validate_align(void* ptr) {
    return ((unsigned int) ptr % ENKI_HEAP_BLOCK_SIZE) == 0;
}

int heap_create(struct heap* heap, void* start_addr, void* end_addr, struct heap_table* table) {    
    int status = OK;
    if (!heap_validate_align(start_addr) || !heap_validate_align(end_addr)) {
        status = -EINVARG;
        goto out;  // bad alignment
    }

    memset(heap, 0x00, sizeof(struct heap));
    heap->start_addr = start_addr;
    heap->table = table;

    status = heap_validate_table(start_addr, end_addr, table);
    if (status < 0) {
        goto out;  // bad table
    }

    // set entire heap as free memory
    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->length;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

out:
    return status;
}

// align value to proper block size
static uint32_t heap_align_val(uint32_t val) {
    if (val % ENKI_HEAP_BLOCK_SIZE == 0) {
        return val;
    }
    val -= (val % ENKI_HEAP_BLOCK_SIZE);
    return val + ENKI_HEAP_BLOCK_SIZE;
}

static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry) {
    return entry & 0x0F; // first nibble
}

// find block where allocation will start
int heap_get_start_block(struct heap* heap, uint32_t total_blocks) {
    struct heap_table* table = heap->table;
    int current = 0;
    int start = -1;

    for (size_t i = 0; i < table->length; i++) {
        if (heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE) {
            current = 0;
            start = -1;
            continue;
        }

        // first block
        if (start == -1) {
            start = i;
        }
        current++;

        if (current == total_blocks) {
            break;  // we found enough blocks
        }
    }
    return (start == -1) ? -ENOMEM : start;
}

// get address from block
void* heap_block_to_address(struct heap* heap, int block) {
    return heap->start_addr + (block * ENKI_HEAP_BLOCK_SIZE);
}

// get block from address
int heap_address_to_block(struct heap* heap, void* addr) {
    return ((int)(addr - heap->start_addr)) / ENKI_HEAP_BLOCK_SIZE;
}

void heap_mark_blocks_taken(struct heap* heap, int start_block, int total_blocks) {
    int end_block = start_block + total_blocks - 1;  // zero indexed
    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;
    
    if (total_blocks > 1) {
        entry |= HEAP_BLOCK_HAS_NEXT;
    }

    // mark each block
    for (int i = start_block; i <= end_block; i++) {
        heap->table->entries[i] = entry;
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;

        if (i != end_block - 1) {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}

void heap_mark_blocks_free(struct heap* heap, int start_block) {
    struct heap_table* table = heap->table;
    for (int i = start_block; i < (int)table->length; i++) {
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;

        if (!(entry & HEAP_BLOCK_HAS_NEXT)) {
            break; // reached end of allocation
        }
    }
}

// allocate blocks from heap
void* heap_malloc_blocks(struct heap* heap, uint32_t total_blocks) {
    void* addr = 0x0;
    int start_block = heap_get_start_block(heap, total_blocks);

    if (start_block < 0) {
        goto out;
    }
    addr = heap_block_to_address(heap, start_block);
    heap_mark_blocks_taken(heap, start_block, total_blocks);
out:
    return addr;
}

void* heap_malloc(struct heap* heap, size_t size) {
    size_t aligned = heap_align_val(size);
    uint32_t total_blocks = aligned / ENKI_HEAP_BLOCK_SIZE;
    return heap_malloc_blocks(heap, total_blocks);
}

void heap_free(struct heap* heap, void* addr) {
    heap_mark_blocks_free(heap, heap_address_to_block(heap, addr));
}
