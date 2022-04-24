#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include <stddef.h>
#include <stdint.h>

#include "../../config.h"
#include "elf.h"

struct elf_file {
    char file_name[ENKI_MAX_PATH];
    int in_mem_size;
    void* elf_memory;      // physical address ELF file is loaded at
    void* virt_base_addr;
    void* virt_end_addr;
    void* phys_base_addr;
    void* phys_end_addr;
};

// TODO: needed?
void* elf_virt_base(struct elf_file* file);

// TODO: needed?
void* elf_virt_end(struct elf_file* file);

// TODO: needed?
void* elf_phys_base(struct elf_file* file);

// TODO: needed?
void* elf_phys_end(struct elf_file* file);

// load ELF file by name
int elf_load(const char* file_name, struct elf_file** file_out);

// close ELF file
void elf_close(struct elf_file* elf_file);

#endif