#ifndef ENKI_H
#define ENKI_H

#include <stdbool.h>
#include <stddef.h>

struct cmd_arg {
    char arg[512];
    struct cmd_arg* next;
};

struct process_args {
    int argc;
    char** argv;
};

// parse shell command
struct cmd_arg* enki_parse_cmd(const char* cmd, int max);

// print message to terminal
void enki_print(const char* msg);

// get key pressed
int enki_getkey();

// print character to terminal
void enki_putchar(char c);

// allocate heap memory
void* enki_malloc(size_t size);

// free allocated heap memory
void enki_free(void* ptr);

// block until key is pressed and return it
int enki_getkey_block();

// read line of input from terminal
void enki_tty_readline(char* out, int max, bool show_typing);

// start a process
void enki_process_start(const char* file_name);

// system command
int enki_system(struct cmd_arg* args);

// get process arguments
void enki_process_args(struct process_args* args);

// 
int enki_system_run(const char* cmd);

// exit current process
void enki_exit();

#endif