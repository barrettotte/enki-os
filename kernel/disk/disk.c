#include "../io/io.h"
#include "../string/string.h"
#include "../include/kernel/config.h"
#include "../include/kernel/status.h"
#include "disk.h"

struct disk disk; // primary hard disk

int disk_read_sector(int lba, int total, void* buf) {
    outb(0x1F6, (lba >> 24) | 0xE0);           // send drive and bits 24-27 of LBA
    outb(0x1F2, total);                        // number of sectors
    outb(0x1F3, (unsigned char)(lba & 0xFF));  // bits 0-7 of LBA
    outb(0x1F4, (unsigned char)(lba >> 8));    // bits 8-15 of LBA
    outb(0x1F5, (unsigned char)(lba >> 16));   // bits 16-23 of LBA
    outb(0x1F7, 0x20);                         // command = read sector(s) with retry

    unsigned short* buf_ptr = (unsigned short*) buf;

    for (int i = 0; i < total; i++) {
        char c = inpb(0x1F7);

        // wait for ready
        while (!(c & 0x08)) {
            c = inpb(0x1F7);
        }
        // copy from hard disk to memory
        for (int i = 0; i < 256; i++) {
            *buf_ptr = inpw(0x1F0);
            buf_ptr++;
        }
    }
    return 0;
}

void disk_search_and_init() {
    // pretending to search for now...only drive 0
    memset(&disk, 0, sizeof(disk));
    disk.type = ENKI_DISK_TYPE_REAL;
    disk.sector_size = ENKI_SECTOR_SIZE;
    disk.id = 0;
    disk.fs = fs_resolve(&disk);
}

struct disk* disk_get(int idx) {
    // no array yet...
    if (idx != 0) {
        return 0; // invalid
    }
    return &disk;
}

int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf) {
    if (idisk != &disk) {
        return -EIO; // only primary disk for now
    }
    return disk_read_sector(lba, total, buf);
}
