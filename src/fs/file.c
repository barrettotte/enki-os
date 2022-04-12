#include "file.h"
#include "../config.h"
#include "../kernel.h"
#include "../status.h"
#include "../memory/heap/kheap.h"
#include "../memory/memory.h"

struct file_system* file_systems[ENKI_MAX_FILE_SYSTEMS];
struct file_descriptor* file_descriptors[ENKI_MAX_FILE_DESCRIPTORS];

// find a free slot in available file systems
static struct file_system** fs_get_free_file_system() {
    for (int i = 0; i < ENKI_MAX_FILE_SYSTEMS; i++) {
        if (file_systems[i] == 0) {
            return &file_systems[i];
        }
    }
    return 0;
}

void fs_insert_file_system(struct file_system* fs) {
    struct file_system** free_fs;
    free_fs = fs_get_free_file_system();

    if (!free_fs) {
        print("Panic! Problem inserting file system, no free fs");
        while(1) {}
    }
    *free_fs = fs;
}

// load file systems built into the kernel
static void fs_static_load() {
    // TODO: fs_insert_file_system(fat16_init());
}

void fs_load() {
    memset(file_systems, 0, sizeof(file_systems));
    fs_static_load();
}

void fs_init() {
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

// create a reference to new file descriptor
static int file_new_descriptor(struct file_descriptor** desc_out) {
    int status = -ENOMEM;
    for (int i = 0; i < ENKI_MAX_FILE_DESCRIPTORS; i++) {
        if (file_descriptors[i] == 0) {
            struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));
            desc->index = i + 1;  // one-indexed
            file_descriptors[i] = desc;
            *desc_out = desc;
            
            status = OK;
            break;
        }
    }
    return status;
}

// get reference to a file descriptor
static struct file_descriptor* file_get_descriptor(int fd) {
    if (fd <= 0 || fd >= ENKI_MAX_FILE_DESCRIPTORS) {
        return 0;
    }
    return file_descriptors[fd - 1]; // one-indexed to zero-indexed
}

struct file_system* fs_resolve(struct disk* disk) {
    struct file_system* fs = 0;
    for (int i = 0; i < ENKI_MAX_FILE_SYSTEMS; i++) {
        if (file_systems[i] != 0 && file_systems[i]->resolve(disk) == 0) {
            fs = file_systems[i];
            break;
        }
    }
    return fs;
}

int fopen(const char* file_name, const char* mode) {
    return -EIO;
}