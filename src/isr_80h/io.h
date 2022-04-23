#ifndef ISR_80H_IO_H
#define ISR_80H_IO_H

struct interrupt_frame;

void* isr_80h_cmd1_print(struct interrupt_frame* frame);
void* isr_80h_cmd2_getkey(struct interrupt_frame* frame);
void* isr_80h_cmd3_putchar(struct interrupt_frame* frame);

#endif