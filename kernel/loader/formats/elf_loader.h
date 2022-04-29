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

// get pointer to file memory
void* elf_memory(struct elf_file* file);

// get pointer to file memory
struct elf_header* elf_header(struct elf_file* file);

// get pointer to section header
struct elf32_shdr* elf_sheader(struct elf_header* header);

// get pointer to program header
struct elf32_phdr* elf_pheader(struct elf_header* header);

// get a particular program header entry
struct elf32_phdr* elf_program_header(struct elf_header* header, int idx);

// get a particular section header entry
struct elf32_shdr* elf_section_header(struct elf_header* header, int idx);

// TODO: needed?
void* elf_virt_base(struct elf_file* file);

// TODO: needed?
void* elf_virt_end(struct elf_file* file);

// TODO: needed?
void* elf_phys_base(struct elf_file* file);

// TODO: needed?
void* elf_phys_end(struct elf_file* file);

// calculate physical address of program header
void* elf_phdr_phys_addr(struct elf_file* ef, struct elf32_phdr* phdr);

// load ELF file by name
int elf_load(const char* file_name, struct elf_file** file_out);

// close ELF file
void elf_close(struct elf_file* elf_file);

#endif