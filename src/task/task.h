#ifndef TASK_H
#define TASK_H

#include "../config.h"
#include "../memory/paging/paging.h"
#include "process.h"

struct registers {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};

struct interrupt_frame;
struct process;

struct task {
    struct paging_4gb_chunk* page_dir;
    struct registers registers;  // registers when task isn't active
    struct task* next;
    struct task* prev;
    struct process* process;
};

// return current task
struct task* task_get_current();

// retrieve next task in tasks list
struct task* task_get_next();

// free a task's resources
int task_free(struct task* task);

// create a new task
struct task* task_new(struct process* process);

// switch to task 
int task_switch(struct task* task);

// leave kernel page directory and load task page directory
int task_page();

// switch to given task's page directory
int task_page_to(struct task* task);

// run very first task
void task_run_first();

//
void task_current_save_state(struct interrupt_frame* frame);

// copy string from task to
int copy_str_from_task(struct task* task, void* virt_addr, void* phys_addr, int max);

// fetch an item from task's stack
void* task_get_stack_item(struct task* task, int idx);

// drop into userland
void task_return(struct registers* regs);

// restore general purpose registers
void restore_gpr(struct registers* regs);

// change all segment registers to user data segment registers
void user_registers();

#endif