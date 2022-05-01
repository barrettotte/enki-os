#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <kernel/idt.h>
#include <kernel/io.h>
#include <kernel/kernel.h>
#include <kernel/panic.h>
#include <kernel/status.h>
#include <kernel/syscall.h>
#include <memory/heap.h>
#include <string.h>
#include <task/process.h>

static SYSCALL syscalls[ENKI_MAX_SYSCALLS];

void syscall_register(int idx, SYSCALL fn) {
    if (idx < 0 || idx >= ENKI_MAX_SYSCALLS) {
        panic("Attempted to register syscall out of bounds.\n");
    }
    if (syscalls[idx]) {
        panic("Attempted to override existing syscall.\n");
    }
    syscalls[idx] = fn;
}

void* syscall_invoke(int idx, struct interrupt_frame* frame) {
    if (idx < 0 || idx >= ENKI_MAX_SYSCALLS) {
        return 0;  // invalid command
    }
    SYSCALL fn = syscalls[idx];
    if (!fn) {
        return 0; // command doesn't exist
    }
    return (void*) fn(frame);
}

void* syscall_handler(int idx, struct interrupt_frame* frame) {
    kernel_page();
    task_current_save_state(frame);
    void* result = syscall_invoke(idx, frame);
    task_page();
    return result;
}

void* syscall_1_print(struct interrupt_frame* frame) {
    void* user_buffer = task_get_stack_item(task_get_current(), 0); // args[0]
    char kernel_buffer[1024];

    copy_str_from_task(task_get_current(), user_buffer, kernel_buffer, sizeof(kernel_buffer));
    tty_writestr(kernel_buffer);
    return 0;
}

void* syscall_2_getkey(struct interrupt_frame* frame) {
    return (void*) ((int) keyboard_pop());
}

void* syscall_3_putchar(struct interrupt_frame* frame) {
    char c = (char)(int) task_get_stack_item(task_get_current(), 0);
    tty_writechar(c, 15);
    return 0;
}

void* syscall_4_malloc(struct interrupt_frame* frame) {
    struct task* task = task_get_current();
    size_t size = (size_t) task_get_stack_item(task, 0);
    return process_malloc(task->process, size);
}

void* syscall_5_free(struct interrupt_frame* frame) {
    struct task* task = task_get_current();
    void* to_free = task_get_stack_item(task, 0);
    process_free(task->process, to_free);
    return 0;
}

void* syscall_6_proc_start(struct interrupt_frame* frame) {
    struct task* task = task_get_current();
    void* file_name_user = task_get_stack_item(task, 0);
    char file_name[ENKI_MAX_PATH];
    
    int result = copy_str_from_task(task, file_name_user, file_name, sizeof(file_name));
    if (result < 0) {
        return (void*) result;
    }

    // TODO: temp - ideally this should be an ENV variable
    char path[ENKI_MAX_PATH];  // note: overflow insanely likely
    strcpy(path, "0:/");
    strcpy(path + 3, file_name);

    // load process
    struct process* proc = 0;
    result = process_load_switch(path, &proc);  // TODO: temp
    if (result < 0) {
        return (void*) result;  // failed to load process
    }

    // start process
    task_switch(proc->task);              // switch to new process's task
    task_return(&proc->task->registers);  // drop into kernel
    return 0;
}

void* syscall_7_system(struct interrupt_frame* frame) {
    struct task* task = task_get_current();
    struct cmd_arg* args = task_virt_to_phys_addr(task, task_get_stack_item(task, 0));
    
    if (!args || strlen(args[0].arg) == 0) {
        return (void*) -EINVARG;
    }
    struct cmd_arg* root_cmd = &args[0];
    const char* pgm_name = root_cmd->arg;

    char path[ENKI_MAX_PATH];
    strcpy(path, "0:/"); // TODO: need ENV variable for drive this is temp
    strncpy(path + 3, pgm_name, sizeof(path));

    struct process* proc = 0;
    int result = process_load_switch(path, &proc);
    if (result < 0) {
        return (void*) result;
    }

    result = process_inject_args(proc, root_cmd);
    if (result < 0) {
        return (void*) result;
    }

    task_switch(proc->task);
    task_return(&proc->task->registers);  // enter userland

    return 0;  // will never actually return - moved to next proc
}

void* syscall_8_pgm_args(struct interrupt_frame* frame) {
    struct task* task = task_get_current();
    struct process* proc = task->process;
    struct process_args* args = task_virt_to_phys_addr(task, task_get_stack_item(task, 0));

    process_get_args(proc, &args->argc, &args->argv);
    return 0;
}

void* syscall_9_exit(struct interrupt_frame* frame) {
    struct process* proc = task_get_current()->process;
    process_terminate(proc);
    task_switch_next();
    // TODO: check proc exit code
    return 0;
}

void syscall_register_all() {
    // 0
    syscall_register(SYSCALL_1_PRINT, syscall_1_print);
    syscall_register(SYSCALL_2_GETKEY, syscall_2_getkey);
    syscall_register(SYSCALL_3_PUTCHAR, syscall_3_putchar);
    syscall_register(SYSCALL_4_MALLOC, syscall_4_malloc);
    syscall_register(SYSCALL_5_FREE, syscall_5_free);
    syscall_register(SYSCALL_6_PROC_START, syscall_6_proc_start);
    syscall_register(SYSCALL_7_SYSTEM, syscall_7_system);
    syscall_register(SYSCALL_8_PGM_ARGS, syscall_8_pgm_args);
    syscall_register(SYSCALL_9_EXIT, syscall_9_exit);
}
