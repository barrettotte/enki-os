#ifndef PS2_KEYBOARD_H
#define PS2_KEYBOARD_H

#define PS2_CMD_PORT          0x64   // write
#define PS2_STATUS_PORT       0x64   // read
#define PS2_DATA_PORT         0x60   // read/write

#define PS2_CMD_EN_FIRST_PORT 0xAE

#define PS2_KEY_RELEASED      0x80

#define ISR_KBD_INTERRUPT     0x21

// initialize new PS/2 keyboard
struct keyboard* ps2_init();

#endif