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
};

// load a process into an open slot
int process_load(const char* file_name, struct process** process);

// load file as process into slot
int process_load_for_slot(const char* file_name, struct process** process, int process_slot);

#endif