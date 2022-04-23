#ifndef ISR_80H_H
#define ISR_80H_H

enum SystemCalls {
    SYSCALL_0_SUM,
    SYSCALL_1_PRINT,
};

// register all system calls
void isr_80h_register_cmds();

#endif