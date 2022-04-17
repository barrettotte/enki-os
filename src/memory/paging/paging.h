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

// free memory used by chunk
void paging_free_4gb(struct paging_4gb_chunk* chunk);

// access directory of chunk
uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk);

// switch page directory
void paging_switch(struct paging_4gb_chunk* directory);

// check if address is page-aligned
bool paging_is_aligned(void* addr);

// enable paging for processor
void enable_paging();

// find directory index and table index for a virtual address
int paging_get_indices(void* virt_addr, uint32_t* dir_idx_out, uint32_t* table_idx_out);

//
// align address to page size
void* paging_align_address(void* addr);

//
int paging_map(struct paging_4gb_chunk* dir, void* virt_addr, void* phys_addr, int flags);

//
int paging_map_range(struct paging_4gb_chunk* dir, void* virt_addr, void* phys_addr, int page_count, int flags);

//
int paging_map_to(struct paging_4gb_chunk* dir, void* virt_addr, void* phys_addr, void* phys_end, int flags);

// set page table entry
int paging_set(uint32_t* directory, void* virt_addr, uint32_t val);

#endif