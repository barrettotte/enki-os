#include "../kernel.h"
#include "../memory/memory.h"
#include "../memory/heap/kheap.h"
#include "../memory/paging/paging.h"
#include "../status.h"
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
    task->page_dir = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    
    if (!task->page_dir) {
        return -EIO;
    }
    task->registers.ip = ENKI_PGM_VIRT_ADDR;
    task->registers.ss = USER_DATA_SEG;
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

    if (!task_head) {
        task_head = task;
        task_tail = task;
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
