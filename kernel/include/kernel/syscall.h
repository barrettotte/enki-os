#ifndef SYSCALL_H
#define SYSCALL_H

struct interrupt_frame;
typedef void*(*SYSCALL)(struct interrupt_frame* frame);

enum SystemCalls {
    SYSCALL_0_RESERVED,
    SYSCALL_1_PRINT,
    SYSCALL_2_GETKEY,
    SYSCALL_3_PUTCHAR,
    SYSCALL_4_MALLOC,
    SYSCALL_5_FREE,
    SYSCALL_6_PROC_START,
    SYSCALL_7_SYSTEM,
    SYSCALL_8_PGM_ARGS,
    SYSCALL_9_EXIT,
};

void* syscall_1_print(struct interrupt_frame* frame);
void* syscall_2_getkey(struct interrupt_frame* frame);
void* syscall_3_putchar(struct interrupt_frame* frame);
void* syscall_4_malloc(struct interrupt_frame* frame);
void* syscall_5_free(struct interrupt_frame* frame);
void* syscall_6_proc_start(struct interrupt_frame* frame);
void* syscall_7_system(struct interrupt_frame* frame);
void* syscall_8_pgm_args(struct interrupt_frame* frame);
void* syscall_9_exit(struct interrupt_frame* frame);

// register all system calls
void syscall_register_all();

// register new system call
void syscall_register(int syscall_idx, SYSCALL syscall_fn);

// wrapper around syscall to get from userland to kernel
void syscall_wrapper();

#endif