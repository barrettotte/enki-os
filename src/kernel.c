#include "config.h"
#include "kernel.h"
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
#include "memory/memory.h"
#include "memory/paging/paging.h"
#include "status.h"
#include "string/string.h"
#include "task/task.h"
#include "task/process.h"
#include "task/tss.h"

#include <stddef.h>
#include <stdint.h>

uint16_t *video_mem = 0;
uint16_t tty_row = 0;
uint16_t tty_col = 0;

// build video memory entry for terminal character
uint16_t tty_newchar(char c, char color)
{
    return (color << 8) | c; // little endian
}

// place character at (x,y) in terminal
void tty_putchar(int x, int y, char c, char color)
{
    video_mem[(y * VGA_WIDTH) + x] = tty_newchar(c, color);
}

// write character to current terminal position
void tty_writechar(char c, char color)
{
    if (c == '\n')
    {
        tty_row++;
        tty_col = 0;
        return;
    }
    tty_putchar(tty_col, tty_row, c, color);
    tty_col++;

    if (tty_col >= VGA_WIDTH)
    {
        tty_col = 0;
        tty_row++;
    }
}

// initialize terminal
void tty_init()
{
    video_mem = (uint16_t *)(0xB8000); // TODO: const?
    tty_row = 0;
    tty_col = 0;

    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            tty_putchar(x, y, ' ', 0);
        }
    }
}

void print(const char *s)
{
    int len = strlen(s);
    for (int i = 0; i < len; i++)
    {
        tty_writechar(s[i], 15);
    }
}

static struct paging_4gb_chunk *kernel_chunk = 0;

void panic(const char* msg) {
    print(msg);
    while(1) {}
}

void kernel_page() {
    kernel_registers();
    paging_switch(kernel_chunk);
}

struct tss tss;
struct gdt gdt_real[ENKI_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[ENKI_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},               // null
    {.base = 0x00, .limit = 0xFFFFFFFF, .type=0x9A},           // kernel code
    {.base = 0x00, .limit = 0xFFFFFFFF, .type=0X92},           // kernel data
    {.base = 0x00, .limit = 0xFFFFFFFF, .type=0xF8},           // user code
    {.base = 0x00, .limit = 0xFFFFFFFF, .type=0xF2},           // user data
    {.base = (uint32_t)&tss, .limit=sizeof(tss), .type=0xE9},  // task switching segment
};

void kernel_main() {
    tty_init();
    print("Welcome to Enki OS\n\n");

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
    kernel_chunk = paging_new_4gb(paging_flags);
    paging_switch(kernel_chunk);
    enable_paging();

    isr_80h_register_cmds();

    keyboard_init();

    // test process
    struct process* process = 0;
    int status = process_load_switch("0:/nothing.bin", &process);
    if (status != OK) {
        panic("Failed to load nothing.bin\n");
    }

    print("Entering first task...\n\n");
    task_run_first();

    // enable_interrupts();

    // test FAT16 file open
    // int fd = fopen("0:/hello.txt", "r");
    // if (fd) {
    //     print("opened 0:/hello.txt\n");
    //     struct file_stat s;
    //     fstat(fd, &s);
    //     fclose(fd);
    //     print("closed 0:/hello.txt\n");
    // }

    print("\nend of kernel_main() reached\n");
}
