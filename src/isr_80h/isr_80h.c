#include "../idt/idt.h"
#include "isr_80h.h"
#include "misc.h"

void isr_80h_register_cmds() {
    isr_80h_register_cmd(SYSCALL_0_SUM, isr_80h_cmd0_sum);
}
