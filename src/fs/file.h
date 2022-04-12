#ifndef FILE_H
#define FILE_H

#include "path.h"

typedef unsigned int FILE_SEEK_MODE;
enum {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

typedef unsigned int FILE_MODE;
enum {
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

struct disk;  // fwd decl
typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk, struct path_part path, FILE_MODE mode);
typedef int (*FS_RESOLVE_FUNCTION)(struct disk* disk);

struct file_system {
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;
    char name[20];
};

struct file_descriptor {
    int index;
    struct file_system fs;
    void* private_data;
    struct disk* disk;
};

// initialize file system
void fs_init();

// 
int fopen(const char* file_name, const char* mode);

// 
void fs_insert_file_system(struct file_system* fs);

//
struct file_system* fs_resolve(struct disk* disk);

#endif