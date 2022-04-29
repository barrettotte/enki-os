#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_CAPSLOCK_OFF 0
#define KEYBOARD_CAPSLOCK_ON  1

typedef int KEYBOARD_CAPSLOCK_STATE;

typedef int (*KEYBOARD_INIT_FN)();

struct keyboard {
    char name[32];
    KEYBOARD_CAPSLOCK_STATE capslock;

    struct keyboard* next;
    KEYBOARD_INIT_FN init;
};

struct process;

// insert a keyboard device
int keyboard_insert(struct keyboard* kbd);

// initialize a new keyboard
void keyboard_init();

// perform backspace action
void keyboard_backspace(struct process* proc);

// push keypress into current process keyboard buffer
void keyboard_push(char c);

// pop keypress from current task keyboard buffer
char keyboard_pop();

// get capslock state of keyboard
KEYBOARD_CAPSLOCK_STATE keyboard_get_capslock(struct keyboard* kbd);

// set capslock state of keyboard
void keyboard_set_capslock(struct keyboard* kbd, KEYBOARD_CAPSLOCK_STATE state);

#endif