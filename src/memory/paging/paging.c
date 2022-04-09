#include "../../status.h"
#include "../heap/kheap.h"
#include "paging.h"

static uint32_t* current_directory = 0;

extern void paging_load_directory(uint32_t* directory);

struct paging_4gb_chunk* paging_new_4gb(uint8_t flags) {
    uint32_t* directory  = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    int offset = 0;

    // create page directory entries
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
        uint32_t* entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);

        // create page table entries
        for (int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; j++) {
            entry[j] = (offset + (j * PAGING_PAGE_SIZE)) | flags;
        }
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        directory[i] = (uint32_t) entry | flags | PAGING_IS_WRITEABLE;
    }
    struct paging_4gb_chunk* chunk = kzalloc(sizeof(struct paging_4gb_chunk));
    chunk->directory_entry = directory;
    
    return chunk;
}

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk) {
    return chunk->directory_entry;
}

void paging_switch(uint32_t* directory) {
    paging_load_directory(directory);
    current_directory = directory;   
}

bool paging_is_aligned(void* addr) {
    return ((uint32_t) addr % PAGING_PAGE_SIZE) == 0;
}

int paging_get_indices(void* virt_addr, uint32_t* dir_idx_out, uint32_t* table_idx_out) {
    int status = OK;
    if (!paging_is_aligned(virt_addr)) {
        status = -EINVARG;
        goto out;
    }
    *dir_idx_out = (uint32_t) virt_addr / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
    *table_idx_out = (uint32_t) virt_addr % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE;
out:
    return status;
}

int paging_set(uint32_t* directory, void* virt_addr, uint32_t val) {
    if (!paging_is_aligned(virt_addr)) {
        return -EINVARG;
    }
    uint32_t dir_idx = 0;
    uint32_t tab_idx = 0;
    int status = paging_get_indices(virt_addr, &dir_idx, &tab_idx);
    if (status < 0) {
        return status;
    }
    
    uint32_t entry = directory[dir_idx];
    uint32_t* table = (uint32_t*)(entry & 0xFFFFF000);  // ignore last 12-bits
    table[tab_idx] = val;

    return OK;
}
