#include "../idt/idt.h"
#include "../task/task.h"
#include "misc.h"

void* isr_80h_cmd0_sum(struct interrupt_frame* frame) {
    int b = (int) task_get_stack_item(task_get_current(), 1);
    int a = (int) task_get_stack_item(task_get_current(), 0);
    return (void*) (a + b);
}
