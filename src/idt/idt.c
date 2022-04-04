#include "idt.h"
#include "../config.h"
#include "../memory/memory.h"

struct idt_entry idt_entries[ENKIOS_TOTAL_INTERRUPTS];
struct idtr_ptr idtr;

// set an interrupt
void idt_set(int i, void* addr) {
    struct idt_entry* entry = &idt_entries[i];
    entry->base_low = (uint32_t) addr & 0x0000FFFF;
    entry->selector = KERNEL_CODE_SEG;
    entry->zero = 0x00;
    entry->
}

// setup IDT
void idt_init() {
    memset(idt_entries, 0, sizeof(idt_entries));
    idtr.limit = sizeof(idt_entries) - 1;
    idtr.base = idt_entries;
}
