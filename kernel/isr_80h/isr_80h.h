#ifndef ISR_80H_H
#define ISR_80H_H

enum SystemCalls {
    SYSCALL_0_SUM,
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

// register all system calls
void isr_80h_register_cmds();

#endif