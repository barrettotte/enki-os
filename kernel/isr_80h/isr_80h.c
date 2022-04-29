#include "../idt/idt.h"
#include "heap.h"
#include "isr_80h.h"
#include "io.h"
#include "misc.h"
#include "process.h"

void isr_80h_register_cmds() {
    isr_80h_register_cmd(SYSCALL_0_SUM, isr_80h_cmd0_sum);
    isr_80h_register_cmd(SYSCALL_1_PRINT, isr_80h_cmd1_print);
    isr_80h_register_cmd(SYSCALL_2_GETKEY, isr_80h_cmd2_getkey);
    isr_80h_register_cmd(SYSCALL_3_PUTCHAR, isr_80h_cmd3_putchar);
    isr_80h_register_cmd(SYSCALL_4_MALLOC, isr_80h_cmd4_malloc);
    isr_80h_register_cmd(SYSCALL_5_FREE, isr_80h_cmd5_free);
    isr_80h_register_cmd(SYSCALL_6_PROC_START, isr_80h_cmd6_proc_start);
    isr_80h_register_cmd(SYSCALL_7_SYSTEM, isr_80h_cmd7_system);
    isr_80h_register_cmd(SYSCALL_8_PGM_ARGS, isr_80h_cmd8_pgm_args);
    isr_80h_register_cmd(SYSCALL_9_EXIT, isr_80h_cmd9_exit);
}
