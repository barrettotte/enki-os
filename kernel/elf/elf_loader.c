#include "../include/elf/elf_loader.h"
#include "../include/kernel/config.h"
#include "../include/kernel/status.h"

#include "../fs/file.h"
#include "../memory/heap/kheap.h"
#include "../memory/paging/paging.h"
#include "../string/string.h"

#include <stdbool.h>

const char elf_magic[] = {0x7F, 'E', 'L', 'F'};

// verify ELF file has valid signature
static bool elf_valid_signature(void* buf) {
    return memcmp(buf, (void*) elf_magic, sizeof(elf_magic)) == 0;
}

// verify ELF header has valid and supported class
static bool elf_valid_class(struct elf_header* header) {
    return header->e_ident[EI_CLASS] == ELFCLASSNONE || header->e_ident[EI_CLASS] == ELFCLASS32;
}

// verify ELF header has valid and supported encoding
static bool elf_valid_encoding(struct elf_header* header) {
    return header->e_ident[EI_DATA] == ELFDATANONE || header->e_ident[EI_DATA] == ELFDATA2LSB;
}

// verify ELF header is executable and loaded at safe address
static bool elf_is_executable(struct elf_header* header) {
    return header->e_type == ET_EXEC && header->e_entry >= ENKI_PGM_VIRT_ADDR;
}

// verify ELF header has a program header
static bool elf_has_program_header(struct elf_header* header) {
    return header->e_phoff != 0;
}

// get pointer to file memory
void* elf_memory(struct elf_file* file) {
    return file->elf_memory;
}

struct elf_header* elf_header(struct elf_file* file) {
    return file->elf_memory;
}

struct elf32_shdr* elf_sheader(struct elf_header* header) {
    return (struct elf32_shdr*)((int)header + header->e_shoff);
}

struct elf32_phdr* elf_pheader(struct elf_header* header) {
    if (header->e_phoff == 0) {
        return 0;
    }
    return (struct elf32_phdr*)((int)header + header->e_phoff);
}

struct elf32_phdr* elf_program_header(struct elf_header* header, int idx) {
    return &elf_pheader(header)[idx];
}

struct elf32_shdr* elf_section_header(struct elf_header* header, int idx) {
    return &elf_sheader(header)[idx];
}

//
char* elf_str_table(struct elf_header* header) {
    return (char*) header + elf_section_header(header, header->e_shstrndx)->sh_offset;
}

void* elf_virt_base(struct elf_file* ef) {
    return ef->virt_base_addr;
}

void* elf_virt_end(struct elf_file* ef) {
    return ef->virt_end_addr;
}

void* elf_phys_base(struct elf_file* ef) {
    return ef->phys_base_addr;
}

void* elf_phys_end(struct elf_file* ef) {
    return ef->phys_end_addr;
}


void* elf_phdr_phys_addr(struct elf_file* ef, struct elf32_phdr* phdr) {
    return elf_memory(ef) + phdr->p_offset;
}

//
int elf_validate_loaded(struct elf_header* header) {
    if (elf_valid_signature(header) && elf_valid_class(header) 
        && elf_valid_encoding(header) && elf_has_program_header(header)) {

        return 0;
    }
    return -EINVFMT;
}

// calculate physical and virtual base/end addresses of program header
int elf_process_phdr_pt_load(struct elf_file* elf_file, struct elf32_phdr* phdr) {
    if ((elf_file->virt_base_addr >= (void*) phdr->p_vaddr) || (elf_file->virt_base_addr == 0x00)) {
        elf_file->virt_base_addr = (void*) phdr->p_vaddr;
        elf_file->phys_base_addr = elf_memory(elf_file) + phdr->p_offset;
    }

    unsigned int end_virt_addr = phdr->p_vaddr + phdr->p_filesz;
    if ((elf_file->virt_end_addr <= (void*) end_virt_addr) || (elf_file->virt_end_addr == 0x00)) {
        elf_file->virt_end_addr = (void*) end_virt_addr;
        elf_file->phys_end_addr = elf_memory(elf_file) + phdr->p_offset + phdr->p_filesz;
    }
    return 0;
}

//
int elf_process_pheader(struct elf_file* elf_file, struct elf32_phdr* phdr) {
    int result = 0;
    switch(phdr->p_type) {
        case PT_LOAD:
            result = elf_process_phdr_pt_load(elf_file, phdr);
            break;
    }
    return result;
}

//
int elf_process_pheaders(struct elf_file* elf_file) {
    int result = 0;
    struct elf_header* header = elf_header(elf_file);

    for (int i = 0; i < header->e_phnum; i++) {
        struct elf32_phdr* phdr = elf_program_header(header, i);
        result = elf_process_pheader(elf_file, phdr);
        
        if (result < 0) {
            break;
        }
    }
    return result;
}

//
int elf_process_loaded(struct elf_file* elf_file) {
    int result = 0;
    struct elf_header* header = elf_header(elf_file);

    result = elf_validate_loaded(header);
    if (result < 0) {
        goto out;
    }

    result = elf_process_pheaders(elf_file);
    if (result < 0) {
        goto out;
    }
out:
    return result;
}

int elf_load(const char* file_name, struct elf_file** file_out) {
    struct elf_file* elf_file = kzalloc(sizeof(struct elf_file));
    int fd = 0;

    int result = fopen(file_name, "r");
    if (result <= 0) {
        result = -EIO;
        goto out;
    }
    fd = result;
    
    struct file_stat stat;
    result = fstat(fd, &stat);
    if (result < 0) {
        goto out;
    }

    elf_file->elf_memory = kzalloc(stat.file_size);
    result = fread(elf_file->elf_memory, stat.file_size, 1, fd);
    if (result < 0) {
        goto out;
    }

    result = elf_process_loaded(elf_file);
    if (result < 0) {
        goto out;
    }
    *file_out = elf_file;

out:
    fclose(fd);
    return result;
}

void elf_close(struct elf_file* elf_file) {
    if (elf_file) {
        kfree(elf_file->elf_memory);
        kfree(elf_file);
    }
}
