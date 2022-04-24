#include "heap.h"
#include "../task/process.h"
#include "../task/task.h"

#include <stddef.h>

void* isr_80h_cmd4_malloc(struct interrupt_frame* frame) {
    struct task* task = task_get_current();
    size_t size = (size_t) task_get_stack_item(task, 0);
    return process_malloc(task->process, size);
}

void* isr_80h_cmd5_free(struct interrupt_frame* frame) {
    struct task* task = task_get_current();
    void* to_free = task_get_stack_item(task, 0);
    process_free(task->process, to_free);
    return 0;
}
