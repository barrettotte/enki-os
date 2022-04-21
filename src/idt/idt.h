// Interrupt Descriptor Table (IDT)
// https://wiki.osdev.org/Interrupt_Descriptor_Table

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct interrupt_frame;
typedef void*(*ISR_80H_CMD)(struct interrupt_frame* frame);

// IDT entry
struct idt_entry {
    uint16_t base_low;   // offset bits 0-15 - lower part of interrupt function offset address
    uint16_t selector;   // code segment selector in GDT or LDT
    uint8_t  reserved;   // reserved = 0x0
    uint8_t  flags;      // stores
                         //   - Type (Gate Type)
                         //   - S (Storage Segment) - set to 0 for interrupt and trap gates
                         //   - DPL (Descriptor Privilege Level) - rings allowed to access this interrupt
                         //   - P (Present) - set 1 for valid descriptor, 0 if unused
    uint16_t base_high;  // offset bits 16-31 - upper part of interrupt function offset address
} __attribute__((packed));

// Stores location of IDT (address and size) in Interrupt Descriptor Table Registor (IDTR)
struct idtr_ptr {
    uint16_t limit;  // size of descriptor table - 1
    uint32_t base;   // base address of IDT
} __attribute__((packed));

// interrupt frame for invoking interrupt 0x80 (syscall)
struct interrupt_frame {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;  // stack pointer from pushad, disregard
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
} __attribute__((packed));

// setup IDT
void idt_init();

// register a new system call
void isr_80h_register_cmd(int cmd_idx, ISR_80H_CMD cmd_fn);

//
void enable_interrupts();

//
void disable_interrupts();

#endif