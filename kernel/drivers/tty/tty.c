#include <drivers/tty.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;
static uint16_t tty_row;
static uint16_t tty_col;

// build video memory entry for terminal character
static uint16_t tty_newchar(char c, char color) {
    return (color << 8) | c; // little endian
}


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

void tty_backspace() {
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

void tty_writechar(char c, char color) {
    if (c == '\n') {
        tty_row++;
        tty_col = 0;
        return;
    } 
    if (c == 0x08) {
        tty_backspace();
        return;
    }

    tty_putchar(tty_col, tty_row, c, color);
    tty_col++;

    if (tty_col >= VGA_WIDTH) {
        tty_col = 0;
        tty_row++;
    }
}

void tty_writestr(const char *s) {
    for (int i = 0; i < strlen(s); i++) {
        tty_writechar(s[i], 15);
    }
}
