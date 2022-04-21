#ifndef ISR_80H_MISC_H
#define ISR_80H_MISC_H

struct interrupt_frame;

void* isr_80h_cmd0_sum(struct interrupt_frame* frame);

#endif