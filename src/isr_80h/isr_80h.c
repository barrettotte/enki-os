#include "../idt/idt.h"
#include "isr_80h.h"
#include "io.h"
#include "misc.h"

void isr_80h_register_cmds() {
    isr_80h_register_cmd(SYSCALL_0_SUM, isr_80h_cmd0_sum);
    isr_80h_register_cmd(SYSCALL_1_PRINT, isr_80h_cmd1_print);
    isr_80h_register_cmd(SYSCALL_2_GETKEY, isr_80h_cmd2_getkey);
    isr_80h_register_cmd(SYSCALL_3_PUTCHAR, isr_80h_cmd3_putchar);
}
