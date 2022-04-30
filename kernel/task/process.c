#include <elf/elf_loader.h>
#include <fs/file.h>
#include <kernel/panic.h>
#include <kernel/status.h>
#include <memory/kheap.h>
#include <memory/paging.h>
#include <string.h>
#include <task/process.h>
#include <task/task.h>

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
    return 0;
}

// load ELF file
static int process_load_elf(const char* file_name, struct process* proc) {
    struct elf_file* elf_file = 0;
    int result = elf_load(file_name, &elf_file);
    if (result < 0) {
        return result;
    }
    proc->file_type = PROCESS_FILE_TYPE_ELF;
    proc->elf_file = elf_file;
    return result;
}

// load binary file
static int process_load_bin(const char* file_name, struct process* proc) {
    int fd = fopen(file_name, "r");
    if (!fd) {
        return -EIO;
    }
    
    struct file_stat stat;
    int result = fstat(fd, &stat);
    if (result) {
        fclose(fd);
        return result;
    }

    void* pgm_data = kzalloc(stat.file_size);
    if (!pgm_data) {
        fclose(fd);
        return -ENOMEM;
    }
    if (fread(pgm_data, stat.file_size, 1, fd) != 1) {
        kfree(pgm_data);
        fclose(fd);
        return -EIO;
    }

    proc->file_type = PROCESS_FILE_TYPE_BIN;
    proc->addr = pgm_data;
    proc->size = stat.file_size;
    return 0;
}

// load data for process
static int process_load_data(const char* file_name, struct process* proc) {
    int result = process_load_elf(file_name, proc);
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
    return 0;
}

// map ELF file process to virtual addresses
static int process_map_elf(struct process* proc) {
    int result = 0;
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
    int result = 0;

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
        return result;
    }

    // setup program stack
    uint32_t flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE;
    void* phys_end = paging_align_address(proc->stack + ENKI_USER_PGM_STACK_SIZE);
    paging_map_to(proc->task->page_dir, (void*) ENKI_PGM_VIRT_STACK_ADDR_END, proc->stack, phys_end, flags);
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
    int slot = process_get_free_slot();
    if (slot < 0) {
        return -EISTKN;
    }
    return process_load_slot(file_name, proc, slot);
}

int process_load_switch(const char* file_name, struct process** proc) {
    int err = process_load(file_name, proc);
    if (!err) {
        process_switch(*proc);
    }
    return err;
}

int process_load_slot(const char* file_name, struct process** proc, int proc_slot) {
    int result = 0;
    struct task* task = 0;
    struct process* load_proc;
    void* pgm_stack_ptr = 0;

    if (process_get(proc_slot) != 0) {
        return -EISTKN;
    }
    
    // load process data
    load_proc = kzalloc(sizeof(struct process));
    if (!load_proc) {
        return -ENOMEM;
    }
    process_init(load_proc);

    result = process_load_data(file_name, load_proc);
    if (result < 0) {
        process_terminate(load_proc);
        return result;
    }

    // load process stack
    pgm_stack_ptr = kzalloc(ENKI_USER_PGM_STACK_SIZE);
    if (!pgm_stack_ptr) {
        process_terminate(load_proc);
        return -ENOMEM;
    }
    load_proc->stack = pgm_stack_ptr;
    strncpy(load_proc->file_name, file_name, sizeof(load_proc->file_name));
    load_proc->id = proc_slot;

    // init task
    task = task_new(load_proc);
    if (!task) {
        process_terminate(load_proc);
        return -ENOMEM;
    }
    load_proc->task = task;

    result = process_map_memory(load_proc);
    if (result < 0) {
        return result;
    }

    *proc = load_proc;
    processes[proc_slot] = load_proc;
    return result;
}

// find a free memory allocation for process
static int process_find_free_alloc(struct process* proc) {
    for (int i = 0; i < ENKI_MAX_PGM_ALLOCATIONS; i++) {
        if (proc->allocations[i].ptr == 0) {
            return i;
        }
    }
    return -ENOMEM;
}

void* process_malloc(struct process* proc, size_t size) {
    void* ptr = kzalloc(size);
    if (!ptr) {
        return 0;
    }
    int idx = process_find_free_alloc(proc);
    if (idx < 0) {
        kfree(ptr);
        return 0;
    }

    int flags = PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;
    void* phys_end = paging_align_address(ptr + size);
    if (paging_map_to(proc->task->page_dir, ptr, ptr, phys_end, flags) < 0) {
        kfree(ptr);
        return 0;
    }
    proc->allocations[idx].ptr = ptr;
    proc->allocations[idx].size = size;
    return ptr;
}

// check if given process owns the provided pointer
static bool process_owns_ptr(struct process* proc, void* ptr) {
    for (int i = 0; i < ENKI_MAX_PGM_ALLOCATIONS; i++) {
        if (proc->allocations[i].ptr == ptr) {
            return true;
        }
    }
    return false;
}

// drop allocation entry from processe's allocation table
static void process_allocation_drop(struct process* proc, void* ptr) {
    for (int i = 0; i < ENKI_MAX_PGM_ALLOCATIONS; i++) {
        if (proc->allocations[i].ptr == ptr) {
            proc->allocations[i].ptr = 0x00;
            proc->allocations[i].size = 0;
        }
    }
}

// get process allocation by address
static struct process_allocation* process_get_allocation(struct process* proc, void* addr) {
    for (int i = 0; i < ENKI_MAX_PGM_ALLOCATIONS; i++) {
        if (proc->allocations[i].ptr == addr) {
            return &proc->allocations[i];
        }
    }
    return 0;
}

void process_free(struct process* proc, void* to_free) {    
    struct process_allocation* alloc = process_get_allocation(proc, to_free);
    if (!alloc) {
        return;  // not this proc's pointer
    }
    void* phys_end = paging_align_address(alloc->ptr + alloc->size);
    if (paging_map_to(proc->task->page_dir, alloc->ptr, alloc->ptr, phys_end, 0x00) < 0) {
        return;
    }
    process_allocation_drop(proc, to_free);
    kfree(to_free);
}

void process_get_args(struct process* proc, int* argc, char*** argv) {
    *argc = proc->args.argc;
    *argv = proc->args.argv;
}

// count arguments in command
int process_count_cmd_args(struct cmd_arg* root_arg) {
    struct cmd_arg* arg = root_arg;
    int i = 0;

    while (arg) {
        i++;
        arg = arg->next;
    }
    return i;
}

int process_inject_args(struct process* proc, struct cmd_arg* root_arg) {
    struct cmd_arg* current = root_arg;
    int i = 0;

    int argc = process_count_cmd_args(root_arg);
    if (argc == 0) {
        return -EIO;
    }
    char **argv = process_malloc(proc, argc * sizeof(const char*));
    if (!argv) {
        return -ENOMEM;
    }

    while(current) {
        char* s_arg = process_malloc(proc, sizeof(current->arg));
        if (!s_arg) {
            return -ENOMEM;
        }
        strncpy(s_arg, current->arg, sizeof(current->arg));
        argv[i] = s_arg;
        current = current->next;
        i++;
    }
    proc->args.argc = argc;
    proc->args.argv = argv;
    return 0;
}

// free all allocations for a process
static int process_free_allocations(struct process* proc) {
    for (int i = 0; i < ENKI_MAX_PGM_ALLOCATIONS; i++) {
        process_free(proc, proc->allocations[i].ptr);
    }
    return 0;
}

// free binary file data
static int process_free_bin_data(struct process* proc) {
    kfree(proc->addr);
    return 0;
}

// free ELF file data
static int process_free_elf_data(struct process* proc) {
    elf_close(proc->elf_file);
    return 0;
}

static int process_free_pgm_data(struct process* proc) {
    int result = 0;
    switch(proc->file_type) {
        case PROCESS_FILE_TYPE_BIN:
            result = process_free_bin_data(proc);
            break;
        case PROCESS_FILE_TYPE_ELF:
            result = process_free_elf_data(proc);
            break;
        default:
            result = -EINVARG;
            break;
    }
    return result;
}

//
void process_switch_to_any() {
    for (int i = 0; i < ENKI_MAX_PROCESSES; i++) {
        if (processes[i]) {
            process_switch(processes[i]);
            return;
        }
    }
    panic("No processes to switch to.\n");  // or respawn a shell
}

// remove process from process linked list
static void process_unlink(struct process* proc) {
    processes[proc->id] = 0x00;
    if (process_current == proc) {
        process_switch_to_any();
    }
}

int process_terminate(struct process* proc) {
    int result = process_free_allocations(proc);
    if (result < 0) {
        return result;
    }
    result = process_free_pgm_data(proc);
    if (result < 0) {
        return result;
    }
    kfree(proc->stack);

    result = task_free(proc->task);
    if (result < 0) {
        return result;
    }
    process_unlink(proc);
    return 0;
}
