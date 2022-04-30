#include <drivers/keyboard.h>
#include <drivers/ps2.h>
#include <kernel/status.h>
#include <task/process.h>
#include <task/task.h>

static struct keyboard* keyboard_head = 0;
static struct keyboard* keyboard_tail = 0;

void keyboard_init() {
    keyboard_insert(ps2_init());
}

int keyboard_insert(struct keyboard* kbd) {
    if (kbd->init == 0) {
        return -EINVARG;  // no init function for keyboard
    }

    if (keyboard_tail) {
        keyboard_tail->next = kbd;
        keyboard_tail = kbd;
    } else {
        keyboard_head = kbd;
        keyboard_tail = kbd;
    }
    return kbd->init();
}

static int keyboard_get_tail_idx(struct process* proc) {
    return proc->keyboard.tail % sizeof(proc->keyboard.buffer);
}

void keyboard_backspace(struct process* proc) {
    proc->keyboard.tail--;
    int idx = keyboard_get_tail_idx(proc);
    proc->keyboard.buffer[idx] = 0x00;
}

void keyboard_push(char c) {
    struct process* proc = process_get_current();
    if (!proc || c == 0x00) {
        return;
    }
    int idx = keyboard_get_tail_idx(proc);
    proc->keyboard.buffer[idx] = c;
    proc->keyboard.tail++;
}

char keyboard_pop() {
    if (!task_get_current()) {
        return 0;
    }
    struct process* proc = task_get_current()->process;
    int idx = proc->keyboard.head % sizeof(proc->keyboard.buffer);
    
    char c = proc->keyboard.buffer[idx];
    if (c == 0x00) {
        return 0; // end of buffer
    }
    proc->keyboard.buffer[idx] = 0;
    proc->keyboard.head++;
    return c;
}

KEYBOARD_CAPSLOCK_STATE keyboard_get_capslock(struct keyboard* kbd) {
    return kbd->capslock;
}

void keyboard_set_capslock(struct keyboard* kbd, KEYBOARD_CAPSLOCK_STATE state) {
    kbd->capslock = state;
}
