#ifndef PAGING_H
#define PAGING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PAGING_CACHE_DISABLE   0b00010000  // PCD
#define PAGING_WRITE_THROUGH   0b00001000  // PWT
#define PAGING_ACCESS_FROM_ALL 0b00000100  // U/S
#define PAGING_IS_WRITEABLE    0b00000010  // R/W
#define PAGING_IS_PRESENT      0b00000001  // P

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE               4096

struct paging_4gb_chunk {
    uint32_t* directory_entry;
};

// create page directory, mapped linearly
struct paging_4gb_chunk* paging_new_4gb(uint8_t flags);

// access directory of chunk
uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk);

// switch page directory
void paging_switch(uint32_t* directory);

// check if address is page-aligned
bool paging_is_aligned(void* addr);

// enable paging for processor
extern void enable_paging();

// find directory index and table index for a virtual address
int paging_get_indices(void* virt_addr, uint32_t* dir_idx_out, uint32_t* table_idx_out);

// set page table entry
int paging_set(uint32_t* directory, void* virt_addr, uint32_t val);

#endif