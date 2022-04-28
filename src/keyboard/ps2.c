#include "../idt/idt.h"
#include "../io/io.h"
#include "../kernel.h"
#include "../task/task.h"
#include "keyboard.h"
#include "ps2.h"

#include <stddef.h>
#include <stdint.h>

#define PS2_CAPSLOCK 0x3A

int ps2_keyboard_init();

static uint8_t scan_set_1[] = {
    0x00, 0x1B, 
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x08, '\t', 
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0x0D, 0x00,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0x00, '\\', 
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0x00, '*', 
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
};

struct keyboard ps2_keyboard = {
    .name = "PS2",
    .init = ps2_keyboard_init
};

// convert keyboard scan code to ASCII char
uint8_t ps2_scan_to_char(uint8_t sc) {
    size_t set_size = sizeof(scan_set_1) / sizeof(uint8_t);
    if (sc > set_size) {
        return 0;    
    }
    char c = scan_set_1[sc];

    if (keyboard_get_capslock(&ps2_keyboard) == KEYBOARD_CAPSLOCK_OFF) {
        if (c >= 'A' && c <= 'Z') {
            c += 32;  // to lower
        }
    }

    return c;
}

void ps2_handle_interrupt() {
    kernel_page();

    uint8_t sc = inpb(PS2_DATA_PORT);
    inpb(PS2_DATA_PORT); // ignore rogue byte sometimes sent

    if (sc & PS2_KEY_RELEASED) {
        return;
    }
    if (sc == PS2_CAPSLOCK) {
        KEYBOARD_CAPSLOCK_STATE state = keyboard_get_capslock(&ps2_keyboard);
        keyboard_set_capslock(&ps2_keyboard, state == KEYBOARD_CAPSLOCK_ON 
            ? KEYBOARD_CAPSLOCK_OFF : KEYBOARD_CAPSLOCK_ON);
    }

    uint8_t c = ps2_scan_to_char(sc);
    if (c != 0) {
        keyboard_push(c); // add to current proc kbd buffer
    }

    task_page();
}

int ps2_keyboard_init() {
    idt_register_int_callback(ISR_KBD_INTERRUPT, ps2_handle_interrupt);
    keyboard_set_capslock(&ps2_keyboard, KEYBOARD_CAPSLOCK_OFF);  // TODO: use kbd cmds to determine init state

    outb(PS2_CMD_PORT, PS2_CMD_EN_FIRST_PORT);
    return 0;
}

struct keyboard* ps2_init() {
    return &ps2_keyboard;
}
