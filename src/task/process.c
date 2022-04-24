#include "../fs/file.h"
#include "../loader/formats/elf_loader.h"
#include "../memory/heap/kheap.h"
#include "../memory/memory.h"
#include "../memory/paging/paging.h"
#include "../string/string.h"
#include "../kernel.h"
#include "../status.h"
#include "process.h"
#include "task.h"

struct process* process_current = 0;
static struct process* processes[ENKI_MAX_PROCESSES] = {};

// initialize a process
static void process_init(struct process* proc) {
    memset(proc, 0, sizeof(struct process));
}

struct process* process_get_current() {
    return process_current;
}

struct process* process_get(int id) {
    if (id < 0 || id >= ENKI_MAX_PROCESSES) {
        return NULL;
    }
    return processes[id];
}

int process_switch(struct process* proc) {
    process_current = proc;
    return OK;
}

// load ELF file
static int process_load_elf(const char* file_name, struct process* proc) {
    int result = 0;
    struct elf_file* elf_file = 0;

    result = elf_load(file_name, &elf_file);
    if (result < 0) {
        goto out;
    }
    proc->file_type = PROCESS_FILE_TYPE_ELF;
    proc->elf_file = elf_file;
out:
    return result;
}

// load binary file
static int process_load_bin(const char* file_name, struct process* proc) {
    int result = OK;
    int fd = fopen(file_name, "r");
    if (!fd) {
        result = -EIO;
        goto out;
    }
    
    struct file_stat stat;
    result = fstat(fd, &stat);
    if (result != OK) {
        goto out;
    }

    void* pgm_data = kzalloc(stat.file_size);
    if (!pgm_data) {
        result = -ENOMEM;
        goto out;
    }

    if (fread(pgm_data, stat.file_size, 1, fd) != 1) {
        result = -EIO;
        goto out;
    }

    proc->file_type = PROCESS_FILE_TYPE_BIN;
    proc->addr = pgm_data;
    proc->size = stat.file_size;
out:
    fclose(fd);
    return result;
}

// load data for process
static int process_load_data(const char* file_name, struct process* proc) {
    int result = OK;
    result = process_load_elf(file_name, proc);
    if (result == -EINVFMT) {
        result = process_load_bin(file_name, proc);
    }
    return result;
}

// map binary file process to virtual addresses
static int process_map_bin(struct process* proc) {
    uint32_t flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE;  // all writeable?

    paging_map_to(proc->task->page_dir, (void*) ENKI_PGM_VIRT_ADDR, 
        proc->addr, paging_align_address(proc->addr + proc->size), flags);
    return OK;
}

// map ELF file process to virtual addresses
static int process_map_elf(struct process* proc) {
    int result = OK;
    struct elf_file* elf_file = proc->elf_file;
    struct elf_header* header = elf_header(elf_file);
    struct elf32_phdr* phdrs = elf_pheader(header);

    for (int i = 0; i < header->e_phnum; i++) {
        struct elf32_phdr* phdr = &phdrs[i];

        int flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;
        if (phdr->p_flags & PF_W) {
            flags |= PAGING_IS_WRITEABLE;
        }
        void* phdr_phys_addr = elf_phdr_phys_addr(elf_file, phdr);
        void* virt_base = paging_align_to_lower((void*) phdr->p_vaddr);
        void* phys_base = paging_align_to_lower(phdr_phys_addr);
        void* phys_end = paging_align_address(phdr_phys_addr + phdr->p_memsz);

        result = paging_map_to(proc->task->page_dir, virt_base, phys_base, phys_end, flags);
        if (result < 0) {
            break;
        }
    }
    return result;
}

// map process to virtual addresses of page tables
int process_map_memory(struct process* proc) {
    int result = OK;

    switch(proc->file_type) {
        case PROCESS_FILE_TYPE_BIN:
            result = process_map_bin(proc);
            break;
        case PROCESS_FILE_TYPE_ELF:
            result = process_map_elf(proc);
            break;
        default:
            panic("process_map_memory: Invalid file type.\n");
            break;
    }
    if (result < 0) {
        goto out;
    }

    // setup program stack
    uint32_t flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE;
    void* phys_end = paging_align_address(proc->stack + ENKI_USER_PGM_STACK_SIZE);
    paging_map_to(proc->task->page_dir, (void*) ENKI_PGM_VIRT_STACK_ADDR_END, proc->stack, phys_end, flags);
out:
    return result;
}

// find a free process slot
int process_get_free_slot() {
    for (int i = 0; i < ENKI_MAX_PROCESSES; i++) {
        if (processes[i] == 0) {
            return i;
        }
    }
    return -EISTKN; // no open slots
}

int process_load(const char* file_name, struct process** proc) {
    int result = OK;
    int slot = process_get_free_slot();
    if (slot < 0) {
        result = -EISTKN;
        goto out;
    }
    result = process_load_slot(file_name, proc, slot);
out:
    return result;
}

int process_load_switch(const char* file_name, struct process** proc) {
    int result = process_load(file_name, proc);
    if (result == OK) {
        process_switch(*proc);
    }
    return result;
}

int process_load_slot(const char* file_name, struct process** proc, int proc_slot) {
    int result = OK;
    struct task* task = 0;
    struct process* load_proc;
    void* pgm_stack_ptr = 0;

    if (process_get(proc_slot) != 0) {
        result = -EISTKN;
        goto out;
    }
    
    // load process data
    load_proc = kzalloc(sizeof(struct process));
    if (!load_proc) {
        result = -ENOMEM;
        goto out;
    }
    process_init(load_proc);
    result = process_load_data(file_name, load_proc);
    if (result < 0) {
        goto out;
    }

    // load process stack
    pgm_stack_ptr = kzalloc(ENKI_USER_PGM_STACK_SIZE);
    if (!pgm_stack_ptr) {
        result = -ENOMEM;
        goto out;
    }
    load_proc->stack = pgm_stack_ptr;
    strncpy(load_proc->file_name, file_name, sizeof(load_proc->file_name));
    load_proc->id = proc_slot;

    // init task
    task = task_new(load_proc);
    if (ERROR_I(task) == 0) {
        result = ERROR_I(task);
        goto out;
    }
    load_proc->task = task;

    result = process_map_memory(load_proc);
    if (result < 0) {
        goto out;
    }

    *proc = load_proc;
    processes[proc_slot] = load_proc;
    
out:
    if (IS_ERR(result)) {
        if (load_proc && load_proc->task) {
            task_free(load_proc->task);
        }
        // TODO: free process data
    }
    return result;
}

// find a free memory allocation for process
static int process_find_free_alloc(struct process* proc) {
    for (int i = 0; i < ENKI_MAX_PGM_ALLOCATIONS; i++) {
        if (proc->allocations[i] == 0) {
            return i;
        }
    }
    return -ENOMEM;
}

void* process_malloc(struct process* proc, size_t size) {
    void* mem = kzalloc(size);
    if (!mem) {
        return 0;
    }

    int idx = process_find_free_alloc(proc);
    if (idx < 0) {
        return 0;
    }
    proc->allocations[idx] = mem;
    return mem;
}

// check if given process owns the provided pointer
static bool process_owns_ptr(struct process* proc, void* ptr) {
    for (int i = 0; i < ENKI_MAX_PGM_ALLOCATIONS; i++) {
        if (proc->allocations[i] == ptr) {
            return true;
        }
    }
    return false;
}

// drop allocation entry from processe's allocation table
static void process_allocation_drop(struct process* proc, void* ptr) {
    for (int i = 0; i < ENKI_MAX_PGM_ALLOCATIONS; i++) {
        if (proc->allocations[i] == ptr) {
            proc->allocations[i] = 0x00;
        }
    }
}

void process_free(struct process* proc, void* to_free) {
    if (!process_owns_ptr(proc, to_free)) {
        return;
    }
    process_allocation_drop(proc, to_free);
    kfree(to_free);
}
