#include <drivers/tty.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "vga.h"

#define KEY_BACKSPACE 0x08

static uint16_t* const VGA_MEMORY = (uint16_t*) VGA_MEMORY_START;
static uint16_t tty_row;
static uint16_t tty_col;

// initialize terminal
void tty_init() {
    tty_row = 0;
    tty_col = 0;

    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            tty_putchar(x, y, ' ', 0);
        }
    }
}

// place character at (x,y) in terminal
void tty_putchar(int x, int y, char c, char color) {
    VGA_MEMORY[(y * VGA_WIDTH) + x] = vga_entry(c, color);
}

static void tty_backspace() {
    if (tty_row == 0 && tty_col == 0) {
        return;
    }
    if (tty_col == 0) {
        tty_row--;
        tty_col = VGA_WIDTH;
    }
    tty_col--;
    tty_writechar(' ', 15);
    tty_col--;
}

static void tty_newline() {
    tty_row++;
    tty_col = 0;
}

static void tty_tab() {
    for (int i = 0; i < 4; i++) {
        tty_writechar(' ', VGA_COLOR_WHITE);
    }
}

void tty_writechar(char c, char color) {
    switch (c) {
        case '\n':
            return tty_newline();
        case '\t':
            return tty_tab();
        case KEY_BACKSPACE:
            return tty_backspace();
    }

    tty_putchar(tty_col, tty_row, c, color);
    tty_col++;

    if (tty_col >= VGA_WIDTH) {
        tty_col = 0;
        tty_row++;
    }
    if (tty_row >= VGA_HEIGHT) {
        // TODO: scroll
    }
}

void tty_writestr(const char *s) {
    for (int i = 0; i < strlen(s); i++) {
        tty_writechar(s[i], VGA_COLOR_WHITE);
    }
}
