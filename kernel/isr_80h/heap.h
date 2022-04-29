#ifndef ISR_80H_HEAP_H
#define ISR_80H_HEAP_H

struct interrupt_frame;

void* isr_80h_cmd4_malloc(struct interrupt_frame* frame);
void* isr_80h_cmd5_free(struct interrupt_frame* frame);

#endif