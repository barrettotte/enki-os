#include "idt.h"
#include "../config.h"
#include "../kernel.h"
#include "../io/io.h"
#include "../memory/memory.h"

struct idt_entry idt_entries[ENKI_TOTAL_INTERRUPTS];
struct idtr_ptr idtr;

extern void idt_load(struct idtr_ptr* ptr);
extern void no_interrupt();
extern void int_21h();

void int_21h_handler() {
    print("Keyboard pressed\n");
    outb(0x20, 0x20);  // relay to PIC
}

void no_interrupt_handler() {
    outb(0x20, 0x20);  // relay to PIC
}

void idt_zero() {
    print("Divide by zero error\n");
}

// set an interrupt
void idt_set(int i, void* addr) {
    struct idt_entry* entry = &idt_entries[i];
    entry->base_low = (uint32_t) addr & 0x0000FFFF;
    entry->selector = KERNEL_CODE_SEG;
    entry->reserved = 0x00;
    entry->flags = 0xEE;  // 11101110 => P, ring 0b11, 0, 32-bit interrupt gate
    entry->base_high = (uint32_t) addr >> 16;
}

void idt_init() {
    memset(idt_entries, 0, sizeof(idt_entries));
    idtr.limit = sizeof(idt_entries) - 1;
    idtr.base = (uint32_t) idt_entries;

    // set interrupts
    for (int i = 0; i < ENKI_TOTAL_INTERRUPTS; i++) {
        idt_set(i, no_interrupt);
    }
    idt_set(0x0, idt_zero);
    idt_set(0x21, int_21h);

    idt_load(&idtr);
}
