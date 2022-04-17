#include "../fs/file.h"
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
static void process_init(struct process* process) {
    memset(process, 0, sizeof(struct process));
}

// fetch current process
struct process* process_get_current() {
    return process_current;
}

struct process* process_get(int id) {
    if (id < 0 || id >= ENKI_MAX_PROCESSES) {
        return NULL;
    }
    return processes[id];
}

// load binary file
static int process_load_bin(const char* file_name, struct process* process) {
    int result = 0;
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

    process->addr = pgm_data;
    process->size = stat.file_size;
out:
    fclose(fd);
    return result;
}

// load data for process
static int process_load_data(const char* file_name, struct process* process) {
    int result = 0;
    result = process_load_bin(file_name, process);
    return result;
}

//
int process_map_bin(struct process* process) {
    int result = 0;
    uint32_t flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE;

    paging_map_to(process->task->page_dir, (void*) ENKI_PGM_VIRT_ADDR, 
        process->addr, paging_align_address(process->addr + process->size), flags);
    return result;
}

// map process to virtual addresses of page tables
int process_map_memory(struct process* process) {
    int result = 0;
    result = process_map_bin(process);
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

int process_load(const char* file_name, struct process** process) {
    int result = 0;
    int slot = process_get_free_slot();
    if (slot < 0) {
        result = -EISTKN;
        goto out;
    }
    result = process_load_for_slot(file_name, process, slot);
out:
    return result;
}

int process_load_for_slot(const char* file_name, struct process** process, int process_slot) {
    int result = 0;
    struct task* task = 0;
    struct process* load_proc;
    void* pgm_stack_ptr = 0;

    if (process_get(process_slot) != 0) {
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
    load_proc->id = process_slot;

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

    *process = load_proc;
    processes[process_slot] = load_proc;
    
out:
    if (IS_ERR(result)) {
        if (load_proc && load_proc->task) {
            task_free(load_proc->task);
        }
        // TODO: free process data
    }
    return result;
}