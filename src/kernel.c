#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include "disk/disk.h"
#include "fs/path_parser.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "string/string.h"

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

void kernel_main()
{
    tty_init();
    print("Welcome to Enki OS\n");
    print("\n\nHello world\n");

    kheap_init();
    disk_search_and_init();
    idt_init();

    // paging
    uint8_t paging_flags = PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;
    kernel_chunk = paging_new_4gb(paging_flags);
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));
    enable_paging();

    struct path_root* root = path_parse("0:/bin/shell.sh", NULL);
    if (root) {

    }

    enable_interrupts();
    print("\n\nend of kernel_main() reached\n");
}
