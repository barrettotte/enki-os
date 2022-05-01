#ifndef PROCESS_H
#define PROCESS_H

#include "../include/kernel/config.h"
#include "task.h"

#include <stdbool.h>
#include <stdint.h>

#define PROCESS_FILE_TYPE_ELF 0
#define PROCESS_FILE_TYPE_BIN 1

typedef unsigned char PROCESS_FILE_TYPE;

struct process_allocation {
    void* ptr;
    size_t size;
};

struct cmd_arg {
    char arg[512];
    struct cmd_arg* next;
};

struct process_args {
    int argc;
    char** argv;
};

struct process {
    uint16_t id;
    char file_name[ENKI_MAX_PATH];
    PROCESS_FILE_TYPE file_type;
    struct task* task;
    struct process_allocation allocations[ENKI_MAX_PGM_ALLOCATIONS];

    union {
        void* addr; // process memory (code and data segments)
        struct elf_file* elf_file;
    };

    void* stack;    // stack memory
    uint32_t size;  // size of process memory

    struct keyboard_buffer {
        char buffer[ENKI_KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;
    } keyboard;

    struct process_args args;
};

// fetch current process
struct process* process_get_current();

// fetch process by id
struct process* process_get(int id);

// switch current process
int process_switch(struct process* proc);

// load a process into an open slot
int process_load(const char* file_name, struct process** proc);

// load process and set it to current process
int process_load_switch(const char* file_name, struct process** proc);

// load file as process into slot
int process_load_slot(const char* file_name, struct process** proc, int proc_slot);

// allocate memory for given process
void* process_malloc(struct process* proc, size_t size);

// free memory in given process
void process_free(struct process* proc, void* to_free);

// get argc and argv of process
void process_get_args(struct process* proc, int* argc, char*** argv);

// inject command arguments into process
int process_inject_args(struct process* proc, struct cmd_arg* root_arg);

// load another shell instance
struct process* process_new_shell();

// terminate a given process and free its memory
int process_terminate(struct process* proc);

#endif