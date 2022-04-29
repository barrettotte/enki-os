#ifndef ISR_80H_PROCESS_H
#define ISR_80H_PROCESS_H

struct interrupt_frame;

void* isr_80h_cmd6_proc_start(struct interrupt_frame* frame);
void* isr_80h_cmd7_system(struct interrupt_frame* frame);
void* isr_80h_cmd8_pgm_args(struct interrupt_frame* frame);
void* isr_80h_cmd9_exit(struct interrupt_frame* frame);

#endif