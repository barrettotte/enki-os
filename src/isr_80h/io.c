#include "../kernel.h"
#include "../task/task.h"
#include "io.h"

void* isr_80h_cmd1_print(struct interrupt_frame* frame) {
    void* user_buffer = task_get_stack_item(task_get_current(), 0); // args[0]
    char kernel_buffer[1024];

    copy_str_from_task(task_get_current(), user_buffer, kernel_buffer, sizeof(kernel_buffer));
    print(kernel_buffer);

    return 0;
}
