#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../string/string.h"
#include "../include/kernel/tty.h"

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;
static uint16_t tty_row;
static uint16_t tty_col;
// static uint8_t tty_color;
// static uint16_t* tty_buffer;

// build video memory entry for terminal character
static uint16_t tty_newchar(char c, char color) {
    return (color << 8) | c; // little endian
}


// initialize terminal
void tty_init() {
    // video_mem = (uint16_t *)(0xB8000); // TODO: const?
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


// static size_t tty_row;
// static size_t tty_column;
// static uint8_t tty_color;
// static uint16_t* tty_buffer;

// void tty_init() {
// 	tty_row = 0;
// 	tty_column = 0;
// 	tty_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
// 	tty_buffer = VGA_MEMORY;

// 	for (size_t y = 0; y < VGA_HEIGHT; y++) {
// 		for (size_t x = 0; x < VGA_WIDTH; x++) {
// 			tty_buffer[y * VGA_WIDTH + x] = vga_entry(' ', tty_color);
// 		}
// 	}
// }

// void tty_setcolor(uint8_t color) {
// 	tty_color = color;
// }

// void tty_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
// 	const size_t index = y * VGA_WIDTH + x;
// 	tty_buffer[index] = vga_entry(c, color);
// }

// void tty_scroll(int line) {
// 	for (size_t i = 0; i < VGA_HEIGHT; i++) {
//         for (size_t j = 0; j < VGA_WIDTH; j++) {
//             tty_buffer[i * VGA_WIDTH + j] = tty_buffer[(i + 1) * VGA_WIDTH + j];
//         }
//     }
// }

// void tty_delete_last_line() {
// 	for (size_t x = 0; x < VGA_WIDTH * 2; x++) {
// 		*(VGA_MEMORY + (VGA_WIDTH * 2) * (VGA_HEIGHT - 1) + x) = 0;
// 	}
// }

// void tty_putchar(char c) {
// 	unsigned char uc = c;

// 	tty_putentryat(uc, tty_color, tty_column, tty_row);
// 	if (++tty_column == VGA_WIDTH) {
// 		tty_column = 0;

// 		if (++tty_row == VGA_HEIGHT) {
// 			for(int line = 1; line <= VGA_HEIGHT - 1; line++) {
// 				tty_scroll(line);
// 			}
// 			tty_delete_last_line();
// 			tty_row = VGA_HEIGHT - 1;
// 		}
// 	}
// }

// void tty_write(const char* s, size_t size) {
// 	for (size_t i = 0; i < size; i++) {
// 		tty_putchar(s[i]);
//     }
// }

// void tty_writestr(const char* s) {
// 	tty_write(s, strlen(s));
// }

// void tty_backspace() {
//     if (tty_row == 0 && tty_column == 0) {
//         return;
//     }
//     if (tty_column == 0) {
//         tty_row--;
//         tty_column = VGA_WIDTH;
//     }
//     tty_column--;
//     tty_putchar(' ');
//     tty_column--;
// }
