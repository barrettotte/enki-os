#include"../config.h"
#include "../memory/heap/kheap.h"
#include "disk_stream.h"

struct disk_stream* disk_stream_new(int disk_id) {
    struct disk* disk = disk_get(disk_id);
    if (!disk) {
        return 0;
    }
    struct disk_stream* stream = kzalloc(sizeof(struct disk_stream));
    stream->pos = 0;
    stream->disk = disk;
    return stream;
}

int disk_stream_seek(struct disk_stream* stream, int pos) {
    stream->pos = pos;
    return 0;
}

int disk_stream_read(struct disk_stream* stream, void* out, int total) {
    int sector = stream->pos / ENKI_SECTOR_SIZE;
    int offset = stream->pos % ENKI_SECTOR_SIZE;
    char buf[ENKI_SECTOR_SIZE];  // TODO: could overflow stack with later recursion...

    int status = disk_read_block(stream->disk, sector, 1, buf);
    if (status < 0) {
        goto out;
    }

    // read requested bytes into output buffer (max of one sector)
    int read = total > ENKI_SECTOR_SIZE ? ENKI_SECTOR_SIZE : total;
    for (int i = 0; i < read; i++) {
        *(char*)out++ = buf[offset + i];
    }
    stream->pos += read;  // reposition stream

    // if reading more than one sector, read from stream again
    if (total > ENKI_SECTOR_SIZE) {
        status = disk_stream_read(stream, out, total - ENKI_SECTOR_SIZE);
    }

out:
    return status;
}

void disk_stream_close(struct disk_stream* stream) {
    kfree(stream);
}