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

struct paging_page_dir {
    uint32_t* entries;
};

// create page directory, mapped linearly
struct paging_page_dir* paging_new(uint8_t flags);

// free memory used by page directory
void paging_free(struct paging_page_dir* dir);

// access directory of page directory
uint32_t* paging_get_directory(struct paging_page_dir* dir);

// switch page directory
void paging_switch(struct paging_page_dir* dir);

// check if address is page-aligned
bool paging_is_aligned(void* addr);

// enable paging for processor
void enable_paging();

// find directory index and table index for a virtual address
int paging_get_indices(void* virt_addr, uint32_t* dir_idx_out, uint32_t* table_idx_out);

// align address to page
void* paging_align_address(void* addr);

// align address to lower page
void* paging_align_to_lower(void* addr);

//
int paging_map(struct paging_page_dir* dir, void* virt_addr, void* phys_addr, int flags);

//
int paging_map_range(struct paging_page_dir* dir, void* virt_addr, void* phys_addr, int page_count, int flags);

//
int paging_map_to(struct paging_page_dir* dir, void* virt_addr, void* phys_addr, void* phys_end, int flags);

// set page table entry at virtual address
int paging_set(uint32_t* dir, void* virt_addr, uint32_t val);

// get page table entry from virtual address
uint32_t paging_get(uint32_t* dir, void* virt_addr);

// get physical address from virtual address
void* paging_get_phys_addr(uint32_t* dir, void* virt_addr);

#endif