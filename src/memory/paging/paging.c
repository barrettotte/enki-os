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

void paging_free_4gb(struct paging_4gb_chunk* chunk) {
    for (int i = 0; i < 1024; i++) {
        uint32_t entry = chunk->directory_entry[i];
        uint32_t* table = (uint32_t*)(entry & 0xFFFFF000);
        kfree(table);
    }
    kfree(chunk->directory_entry);
    kfree(chunk);
}

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk) {
    return chunk->directory_entry;
}

void paging_switch(struct paging_4gb_chunk* directory) {
    paging_load_directory(directory->directory_entry);
    current_directory = directory->directory_entry;
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

void* paging_align_address(void* addr) {
    if((uint32_t) addr % PAGING_PAGE_SIZE) {
        return (void*)((uint32_t) addr + PAGING_PAGE_SIZE - ((uint32_t) addr % PAGING_PAGE_SIZE));
    }
    return addr;
}

int paging_map(struct paging_4gb_chunk* dir, void* virt_addr, void* phys_addr, int flags) {
    if (((unsigned int) virt_addr % PAGING_PAGE_SIZE) || ((unsigned int) phys_addr % PAGING_PAGE_SIZE)) {
        return -EINVARG;
    }
    return paging_set(dir->directory_entry, virt_addr, (uint32_t) phys_addr | flags);
}

int paging_map_range(struct paging_4gb_chunk* dir, void* virt_addr, void* phys_addr, int page_count, int flags) {
    int result = 0;
    for (int i = 0; i < page_count; i++) {
        result = paging_map(dir, virt_addr, phys_addr, flags);
        if (result < 0) {
            break;
        }
        virt_addr += PAGING_PAGE_SIZE;
        phys_addr += PAGING_PAGE_SIZE;
    }
    return result;
}

int paging_map_to(struct paging_4gb_chunk* dir, void* virt_addr, void* phys_addr, void* phys_end, int flags) {
    int result = 0;

    if (((uint32_t) virt_addr % PAGING_PAGE_SIZE) || ((uint32_t) phys_addr % PAGING_PAGE_SIZE)) {
        result = -EINVARG;
        goto out;
    }
    if (((uint32_t) phys_end % PAGING_PAGE_SIZE) || ((uint32_t) phys_end < (uint32_t) phys_addr)) {
        result = -EINVARG;
        goto out;
    }
    uint32_t bytes = phys_end - phys_addr;
    int pages = bytes / PAGING_PAGE_SIZE;
    result = paging_map_range(dir, virt_addr, phys_addr, pages, flags);

out:
    return result;
}

int paging_set(uint32_t* dir, void* virt_addr, uint32_t val) {
    if (!paging_is_aligned(virt_addr)) {
        return -EINVARG;
    }
    uint32_t dir_idx = 0;
    uint32_t tab_idx = 0;
    int result = paging_get_indices(virt_addr, &dir_idx, &tab_idx);
    if (result < 0) {
        return result;
    }
    
    uint32_t entry = dir[dir_idx];
    uint32_t* table = (uint32_t*)(entry & 0xFFFFF000);  // ignore last 12-bits
    table[tab_idx] = val;
    return OK;
}

uint32_t paging_get(uint32_t* dir, void* virt_addr) {
    uint32_t dir_idx = 0;
    uint32_t tab_idx = 0;
    paging_get_indices(virt_addr, &dir_idx, &tab_idx);

    uint32_t entry = dir[dir_idx];
    uint32_t* table = (uint32_t*)(entry &0xFFFFF000);
    return table[tab_idx];
}
