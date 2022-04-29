#include "enki.h"
#include "string.h"

struct cmd_arg* enki_parse_cmd(const char* cmd, int max) {
    struct cmd_arg* root_cmd = 0;
    char scmd[1025];

    if (max >= (int) sizeof(scmd)) {
        return 0;
    }
    
    strncpy(scmd, cmd, sizeof(scmd));
    char* tok = strtok(scmd, " ");
    if (!tok) {
        return root_cmd;
    }

    root_cmd = enki_malloc(sizeof(struct cmd_arg));
    if (!root_cmd) {
        return root_cmd;
    }
    strncpy(root_cmd->arg, tok, sizeof(root_cmd->arg));
    root_cmd->next = 0;

    struct cmd_arg* curr_cmd = root_cmd;
    tok = strtok(NULL, " ");
    while (tok != 0) {
        struct cmd_arg* new_cmd = enki_malloc(sizeof(struct cmd_arg));
        if (!new_cmd) {
            break;
        }
        strncpy(new_cmd->arg, tok, sizeof(new_cmd->arg));
        new_cmd->next = 0;
        curr_cmd->next = new_cmd;
        curr_cmd = new_cmd;
        tok = strtok(NULL, " ");
    }
    return root_cmd;
}

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

int enki_system_run(const char* cmd) {
    char buf[1024];
    strncpy(buf, cmd, sizeof(buf));

    struct cmd_arg* root_cmd = enki_parse_cmd(buf, sizeof(buf));
    if (!root_cmd) {
        return -1;
    }
    return enki_system(root_cmd);
}
