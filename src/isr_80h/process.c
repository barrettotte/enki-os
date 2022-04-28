#include "../config.h"
#include "../status.h"
#include "../string/string.h"
#include "../task/process.h"
#include "../task/task.h"
#include "process.h"

void* isr_80h_cmd6_proc_start(struct interrupt_frame* frame) {
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

void* isr_80h_cmd7_system(struct interrupt_frame* frame) {
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

void* isr_80h_cmd8_pgm_args(struct interrupt_frame* frame) {
    struct task* task = task_get_current();
    struct process* proc = task->process;
    struct process_args* args = task_virt_to_phys_addr(task, task_get_stack_item(task, 0));

    process_get_args(proc, &args->argc, &args->argv);
    return 0;
}

void* isr_80h_cmd9_exit(struct interrupt_frame* frame) {
    struct process* proc = task_get_current()->process;
    process_terminate(proc);
    task_switch_next();
    // TODO: proc exit code
    return 0;
}
