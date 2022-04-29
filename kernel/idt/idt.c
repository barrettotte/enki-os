#include "idt.h"
#include "../config.h"
#include "../kernel.h"
#include "../io/io.h"
#include "../status.h"
#include "../string/string.h"
#include "../task/task.h"

struct idt_entry idt_entries[ENKI_TOTAL_INTERRUPTS];
struct idtr_ptr idtr;

extern void* interrupt_ptr_table[ENKI_TOTAL_INTERRUPTS];
extern void idt_load(struct idtr_ptr* ptr);
extern void no_interrupt();
extern void int_21h();
extern void isr_80h_wrapper();

static INTERRUPT_CALLBACK_FN int_callbacks[ENKI_TOTAL_INTERRUPTS];
static ISR_80H_CMD isr_80h_cmds[ENKI_MAX_ISR_80H_CMD];

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
    idt_set(0x80, isr_80h_wrapper);

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
    return OK;
}

void isr_80h_register_cmd(int cmd_idx, ISR_80H_CMD cmd_fn) {
    if (cmd_idx < 0 || cmd_idx >= ENKI_MAX_ISR_80H_CMD) {
        panic("Attempted to register command out of bounds.\n");
    }
    if (isr_80h_cmds[cmd_idx]) {
        panic("Attempted to override existing command.\n");
    }
    isr_80h_cmds[cmd_idx] = cmd_fn;
}

void* isr_80h_handle_cmd(int cmd_idx, struct interrupt_frame* frame) {
    void* result = 0;

    if (cmd_idx < 0 || cmd_idx >= ENKI_MAX_ISR_80H_CMD) {
        return 0;  // invalid command
    }
    
    ISR_80H_CMD cmd_fn = isr_80h_cmds[cmd_idx];
    if (!cmd_fn) {
        return 0; // command doesn't exist
    }
    result = cmd_fn(frame);
    return result;
}

void* isr_80h_handler(int cmd_idx, struct interrupt_frame* frame) {
    void* result = 0;
    kernel_page();
    task_current_save_state(frame);
    result = isr_80h_handle_cmd(cmd_idx, frame);
    task_page();
    return result;
}
