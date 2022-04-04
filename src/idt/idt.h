// Interrupt Descriptor Table (IDT)

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// IDT entry
struct idt_entry {
    uint16_t base_low;  // offset bits 0-15
    uint16_t selector;  // code segment selector in GDT or LDT
    uint8_t  zero;      // unused = 0
    uint8_t  flags;     // stores
                        //   - P (Present) - TODO:
                        //   - DPL (Descriptor Privilege Level) - TODO:
                        //   - S (Storage Segment) - TODO:
                        //   - Type (Gate Type) - TODO:
    uint16_t base_high;  // offset bits 16-31
} __attribute__((packed));

// Stores location of IDT (address and size) in Interrupt Descriptor Table Registor (IDTR)
struct idtr_ptr {
    uint16_t limit;  // size of descriptor table - 1
    uint32_t base;   // base address of IDT
} __attribute__((packed));

#endif