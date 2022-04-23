#ifndef PROCESS_H
#define PROCESS_H

#include "../config.h"
#include "task.h"

#include <stdint.h>

struct process {
    uint16_t id;
    char file_name[ENKI_MAX_PATH];
    struct task* task;
    void* allocations[ENKI_MAX_PGM_ALLOCATIONS];
    void* addr;     // process memory (code and data segments)
    void* stack;    // stack memory
    uint32_t size;  // size of process memory
    struct keyboard_buffer {
        char buffer[ENKI_KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;
    } keyboard;
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

#endif