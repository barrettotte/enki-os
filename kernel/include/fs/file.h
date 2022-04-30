#ifndef FILE_H
#define FILE_H

#include "path.h"

#include <stdint.h>

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

typedef unsigned int FILE_STAT_FLAGS;
enum {
    FILE_STAT_READ_ONLY = 0b00000001
};

struct disk;
struct file_stat;

typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk, struct path_part* path, FILE_MODE mode);
typedef int (*FS_RESOLVE_FUNCTION)(struct disk* disk);
typedef int (*FS_READ_FUNCTION)(struct disk* disk, void* private, uint32_t size, uint32_t blocks, char* out);
typedef int (*FS_CLOSE_FUNCTION)(void* private);
typedef int (*FS_SEEK_FUNCTION)(void* private, uint32_t offset, FILE_SEEK_MODE mode);
typedef int (*FS_STAT_FUNCTION)(struct disk* disk, void* private, struct file_stat* stat);

struct file_system {
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;
    FS_READ_FUNCTION read;
    FS_CLOSE_FUNCTION close;
    FS_SEEK_FUNCTION seek;
    FS_STAT_FUNCTION stat;
    char name[20];
};

struct file_descriptor {
    int index;
    struct file_system* fs;
    void* private_data;
    struct disk* disk;
};

struct file_stat {
    FILE_STAT_FLAGS flags;
    uint32_t file_size;
};

// initialize file system
void fs_init();

// 
int fopen(const char* file_name, const char* mode);

// read from file into buffer
int fread(void* buf, uint32_t size, uint32_t blocks, int fd);

// close file
int fclose(int fd);

// reposition to offset in file
int fseek(int fd, int offset, FILE_SEEK_MODE mode);

// fetch stats about file
int fstat(int fd, struct file_stat* stat);

// attempt to insert new file system instance into file system list
void fs_insert_file_system(struct file_system* fs);

//
struct file_system* fs_resolve(struct disk* disk);

#endif