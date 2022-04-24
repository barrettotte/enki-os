#include "enki.h"

int enki_getkey_block() {
    int val = 0;
    do {
        val = enki_getkey();
    } while (val == 0);
    return val;
}

void enki_tty_readline(char* out, int max, bool show_typing) {
    int i = 0;
    for (i = 0; i < max - 1; i++) {
        char key = enki_getkey_block();
        
        if (key == 0x0D) {
            break; // carriage return
        }
        if (show_typing) {
            enki_putchar(key);
        }

        // backspace
        if (key == 0x08 && i >= 1) {
            out[i - 1] = 0x00;
            i -= 2;
            continue;  // TODO: could probably do else and i--
        }
        out[i] = key;
    }
    out[i] = 0x00;
}
