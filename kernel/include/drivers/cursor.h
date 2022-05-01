#ifndef CURSOR_H
#define CURSOR_H

#include <stdint.h>

// enable cursor
void cursor_enable(uint8_t start, uint8_t end);

// disable cursor
void cursor_disable();

// update cursor position
void cursor_set_position(int x, int y);

// get cursor's current position
void cursor_get_position();

#endif