#ifndef DISK_STREAM_H
#define DISK_STREAM_H

#include "disk.h"

// stream of bytes from disk
struct disk_stream {
    int pos;
    struct disk* disk;
};

// create a new disk stream for target disk
struct disk_stream* disk_stream_new(int disk_id);

// reposition to given position within disk stream
int disk_stream_seek(struct disk_stream* stream, int pos);

// read bytes from disk stream into output pointer
int disk_stream_read(struct disk_stream* stream, void* out, int total);

// release disk stream's allocated memory
void disk_stream_close(struct disk_stream* stream);

#endif