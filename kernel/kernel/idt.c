#include <kernel/config.h>
#include <kernel/idt.h>
#include <kernel/io.h>
#include <kernel/kernel.h>
#include <kernel/panic.h>
#include <kernel/status.h>
#include <kernel/syscall.h>
#include <drivers/tty.h>
#include <string.h>
#include <task/task.h>

struct idt_entry idt_entries[ENKI_TOTAL_INTERRUPTS];
struct idtr_ptr idtr;

extern void* interrupt_ptr_table[ENKI_TOTAL_INTERRUPTS];
extern void idt_load(struct idtr_ptr* ptr);
extern void no_interrupt();
extern void int_21h();

static INTERRUPT_CALLBACK_FN int_callbacks[ENKI_TOTAL_INTERRUPTS];

void no_interrupt_handler() {
    outb(0x20, 0x20);  // relay to PIC
}

// entry point for handling all interrupts
void interrupt_handler(int interrupt, struct  interrupt_frame* frame) {
    kernel_page();
    if (int_callbacks[interrupt] != 0) {
        task_current_save_state(frame);
        int_callbacks[interrupt](frame);
    }
    task_page();
    outb(0x20, 0x20);  // relay to PIC
}

void idt_zero() {
    tty_writestr("Divide by zero error\n");
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

void idt_handle_exception() {
    process_terminate(task_get_current()->process);
    // TODO: https://wiki.osdev.org/Exceptions
    task_switch_next();
}

void idt_clock() {
    outb(0x20, 0x20);  // ack
    task_switch_next();
}

void idt_init() {
    memset(idt_entries, 0, sizeof(idt_entries));
    idtr.limit = sizeof(idt_entries) - 1;
    idtr.base = (uint32_t) idt_entries;

    // init interrupts
    for (int i = 0; i < ENKI_TOTAL_INTERRUPTS; i++) {
        idt_set(i, interrupt_ptr_table[i]);
    }

    // override interrupts
    idt_set(0x0, idt_zero);
    idt_set(0x80, syscall_wrapper);

    // map exception interrupts
    for (int i = 0; i < 0x20; i++) {
        idt_register_int_callback(i, idt_handle_exception);
    }
    idt_register_int_callback(0x20, idt_clock);

    idt_load(&idtr);
}

int idt_register_int_callback(int interrupt, INTERRUPT_CALLBACK_FN callback) {
    if (interrupt < 0 || interrupt >= ENKI_TOTAL_INTERRUPTS) {
        return -EINVARG;
    }
    int_callbacks[interrupt] = callback;
    return 0;
}
