#include "../kernel.h"
#include "../idt/idt.h"
#include "../memory/memory.h"
#include "../memory/heap/kheap.h"
#include "../memory/paging/paging.h"
#include "../status.h"
#include "../string/string.h"
#include "task.h"

struct task* task_current = 0;
struct task* task_head = 0;
struct task* task_tail = 0;

struct task* task_get_current() {
    return task_current;
}

struct task* task_get_next() {
    if (!task_current->next) {
        return task_head;  // loop back to start
    }
    return task_current->next;
}

// remove task from linked list of tasks
static void task_list_remove(struct task* task) {
    if (task->prev) {
        task->prev->next = task->next;
    }

    if (task == task_head) {
        task_head = task->next;
    } else if (task == task_tail) {
        task_tail = task->prev;
    } else if (task == task_current) {
        task_current = task_get_next();
    }
}

int task_free(struct task* task) {
    paging_free_4gb(task->page_dir);
    task_list_remove(task);
    kfree(task);
    return 0;
}

// initialize a task
int task_init(struct task* task, struct process* process) {
    memset(task, 0, sizeof(struct task));
    task->page_dir = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL); // TODO: secure (ALL=bad)
    
    if (!task->page_dir) {
        return -EIO;
    }
    task->registers.ip = ENKI_PGM_VIRT_ADDR;
    task->registers.ss = USER_DATA_SEG;
    task->registers.cs = USER_CODE_SEG;
    task->registers.esp = ENKI_PGM_VIRT_STACK_ADDR_START;
    task->process = process;

    return 0;
}

struct task* task_new(struct process* process) {
    int result = 0;

    struct task* task = kzalloc(sizeof(struct task));
    if (!task) {
        result = -ENOMEM;
        goto out;
    }
    result = task_init(task, process);
    if (result != OK) {
        goto out;
    }

    if (task_head == 0) {
        task_head = task;
        task_tail = task;
        task_current = task;
        goto out;
    }
    task_tail->next = task;
    task->prev = task_tail;
    task_tail = task;

out:
    if (IS_ERR(result)) {
        task_free(task);
        return ERROR(result);
    }
    return task;
}

int task_switch(struct task* task) {
    task_current = task;
    paging_switch(task->page_dir);
    return 0;
}

int task_page() {
    user_registers();
    task_switch(task_current);
    return 0;
}

int task_page_to(struct task* task) {
    user_registers();
    paging_switch(task->page_dir);
    return 0;
}

void task_run_first() {
    if (!task_current) {
        panic("task_run_first(): No current task\n");
    }
    task_switch(task_head);
    task_return(&task_head->registers);
}

void task_save_state(struct task* task, struct interrupt_frame* frame) {
    task->registers.ip = frame->ip;
    task->registers.cs = frame->cs;
    task->registers.flags = frame->flags;
    task->registers.esp = frame->esp;
    task->registers.ss = frame->ss;
    task->registers.eax = frame->eax;
    task->registers.ebp = frame->ebp;
    task->registers.ebx = frame->ebx;
    task->registers.ecx = frame->ecx;
    task->registers.edi = frame->edi;
    task->registers.edx = frame->edx;
    task->registers.esi = frame->esi;
}

void task_current_save_state(struct interrupt_frame* frame) {
    if (!task_current) {
        panic("No current task to save\n");
    }
    struct task* task = task_current;
    task_save_state(task, frame);
}

int copy_str_from_task(struct task* task, void* virt_addr, void* phys_addr, int max) {
    int result = 0;

    if (max >= PAGING_PAGE_SIZE) {
        result = -EINVARG;
        goto out;
    }

    char* tmp = kzalloc(max);
    if (!tmp) {
        result = -ENOMEM;
        goto out;
    }
    uint32_t* task_dir = task->page_dir->directory_entry;
    uint32_t old_entry = paging_get(task_dir, tmp);
    
    paging_map(task->page_dir, tmp, tmp, PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(task->page_dir);
    strncpy(tmp, virt_addr, max); // copy from user space task to kernel space buffer
    kernel_page();

    result = paging_set(task_dir, tmp, old_entry); // restore task's page directory entry
    if (result < 0) {
        result = -EIO;
        goto out_free;
    }
    strncpy(phys_addr, tmp, max); // copy from buffer to physical address

out_free:
    kfree(tmp);
out:
    return result;
}

void* task_get_stack_item(struct task* task, int idx) {
    void* result = 0;
    uint32_t* sp = (uint32_t*) task->registers.esp;
    task_page_to(task);
    result = (void*) sp[idx];

    kernel_page();
    return result;
}