#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef int (*KEYBOARD_INIT_FN)();

struct keyboard {
    char name[32];
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

#endif