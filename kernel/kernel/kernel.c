#include "include/kernel/config.h"
#include "include/kernel/kernel.h"
#include "include/kernel/panic.h"
#include "idt/idt.h"
#include "io/io.h"
#include "disk/disk.h"
#include "disk/disk_stream.h"
#include "fs/path.h"
#include "fs/file.h"
#include "gdt/gdt.h"
#include "isr_80h/isr_80h.h"
#include "keyboard/keyboard.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "include/kernel/status.h"
#include "string/string.h"
#include "task/task.h"
#include "task/process.h"
#include "task/tss.h"

#include "include/kernel/tty.h"

#include <stddef.h>
#include <stdint.h>

static struct paging_chunk *kernel_chunk = 0;
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

static void test_multitask() {
    struct process* process = 0;
    int status = 0;
    struct cmd_arg arg;

    // 1st process
    status = process_load_switch("0:/nothing", &process);
    if (status) {
        panic("Failed to load file nothing\n");
    }
    strcpy(arg.arg, "X");
    arg.next = 0x00;
    process_inject_args(process, &arg);

    // 2nd process
    status = process_load_switch("0:/nothing", &process);
    if (status) {
        panic("Failed to load file nothing\n");
    }
    strcpy(arg.arg, "A");
    arg.next = 0x00;
    process_inject_args(process, &arg);
}

static void load_shell() {
    struct process* proc = 0;

    if (process_load_switch("0:/shell", &proc)) {
        panic("Failed to load shell process\n");
    }
}

void kernel_page() {
    kernel_registers();
    paging_switch(kernel_chunk);
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
    kernel_chunk = paging_new(paging_flags);
    paging_switch(kernel_chunk);
    enable_paging();

    isr_80h_register_cmds();

    keyboard_init();

    test_multitask(); // TODO: test
    // load_shell();
    task_run_first();
}
