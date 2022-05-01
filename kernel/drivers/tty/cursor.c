#include <kernel/io.h>

#include "vga.h"

void cursor_enable(uint8_t start, uint8_t end) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inpb(0x3D5) & 0xC0) | start);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inpb(0x3D5) & 0xE0) | end);
}

void cursor_disable() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void cursor_set_position(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

uint16_t cursor_get_position() {
    outb(0x3D4, 0x0F);
    uint16_t pos = inpb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inpb(0x3D5)) << 8;
    return pos;
}
