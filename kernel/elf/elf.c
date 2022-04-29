#include "../include/elf/elf.h"

void* elf_get_entry_ptr(struct elf_header* header) {
    return (void*) header->e_entry;
}

uint32_t elf_get_entry(struct elf_header* header) {
    return header->e_entry;
}
