#ifndef DISK_H
#define DISK_H

#include "../fs/file.h"

typedef unsigned int ENKI_DISK_TYPE;

// real physical hard disk
#define ENKI_DISK_TYPE_REAL 0

struct disk {
    ENKI_DISK_TYPE type;
    int sector_size;
    int id;
    struct file_system* fs;
    void* fs_private;
};

// find available disks and init references
void disk_search_and_init();

// fetch reference to disk
struct disk* disk_get(int idx);

// read blocks from disk
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf);

#endif