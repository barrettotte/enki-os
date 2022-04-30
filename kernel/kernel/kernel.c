#include <disk/disk.h>
#include <disk/disk_stream.h>
#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <fs/file.h>
#include <fs/path.h>
#include <kernel/config.h>
#include <kernel/kernel.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/io.h>
#include <kernel/panic.h>
#include <kernel/status.h>
#include <kernel/syscall.h>
#include <memory/kheap.h>
#include <memory/paging.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <task/task.h>
#include <task/process.h>
#include <task/tss.h>

static struct paging_page_dir *kernel_page_dir = 0;
static struct tss tss;
static struct gdt gdt_real[ENKI_TOTAL_GDT_SEGMENTS];
static struct gdt_structured gdt_structured[ENKI_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},               // null
    {.base = 0x00, .limit = 0xFFFFFFFF, .type=0x9A},           // kernel code
    {.base = 0x00, .limit = 0xFFFFFFFF, .type=0X92},           // kernel data
    {.base = 0x00, .limit = 0xFFFFFFFF, .type=0xF8},           // user code
    {.base = 0x00, .limit = 0xFFFFFFFF, .type=0xF2},           // user data
    {.base = (uint32_t)&tss, .limit=sizeof(tss), .type=0xE9},  // task switching segment
};

// simple test for multitasking - will alternate 'X' and 'A' endlessly via timer
static void test_multitask() {
    struct process* proc = 0;
    struct cmd_arg arg;

    // 1st process
    if (process_load_switch("0:/nothing", &proc)) {
        panic("Failed to load file nothing\n");
    }
    strcpy(arg.arg, "X");
    arg.next = 0;
    process_inject_args(proc, &arg);

    // 2nd process
    if (process_load_switch("0:/nothing", &proc)) {
        panic("Failed to load file nothing\n");
    }
    strcpy(arg.arg, "A");
    arg.next = 0;
    process_inject_args(proc, &arg);
}

static void load_shell() {
    struct process* proc = 0;
    if (process_load_switch("0:/shell", &proc)) {
        panic("Failed to load shell process\n");
    }
}

void kernel_page() {
    kernel_registers();
    paging_switch(kernel_page_dir);
}

void kernel_main() {
    tty_init();

    // init GDT
    memset(gdt_real, 0x00, sizeof(gdt_real));
    gdt_unstructure(gdt_real, gdt_structured, ENKI_TOTAL_GDT_SEGMENTS);
    gdt_load(gdt_real, sizeof(gdt_real));

    kheap_init();
    fs_init();
    disk_search_and_init();
    idt_init();

    // init TSS
    memset(&tss, 0x00, sizeof(tss));
    tss.esp0 = 0x600000;
    tss.ss0 = KERNEL_DATA_SEG;
    tss_load(0x28);

    // paging
    uint8_t paging_flags = PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;
    kernel_page_dir = paging_new(paging_flags);
    paging_switch(kernel_page_dir);
    enable_paging();

    syscall_register_all();
    keyboard_init();

    load_shell();
    task_run_first();
}
