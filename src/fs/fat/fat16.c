#include <stdint.h>

#include "../../disk/disk.h"
#include "../../disk/disk_stream.h"
#include "../../memory/heap/kheap.h"
#include "../../memory/memory.h"
#include "../../status.h"
#include "../../string/string.h"
#include "fat16.h"

#define ENKI_FAT16_SIGNATURE  0x29
#define ENKI_FAT16_ENTRY_SIZE 0x02
#define ENKI_FAT16_BAD_SECTOR 0xFF7
#define ENKI_FAT16_UNUSED     0x00

#define FAT_FILE_READ_ONLY    0x01
#define FAT_FILE_HIDDEN       0x02
#define FAT_FILE_SYSTEM       0x04
#define FAT_FILE_VOLUME_LABEL 0x08
#define FAT_FILE_SUBDIRECTORY 0x10
#define FAT_FILE_ARCHIVED     0x20
#define FAT_FILE_DEVICE       0x40
#define FAT_FILE_RESERVED     0x80

#define FAT_ITEM_TYPE_DIR  0
#define FAT_ITEM_TYPE_FILE 1

typedef unsigned int FAT_ITEM_TYPE;

struct fat_header {
    uint8_t short_jmp[3];
    uint8_t oem[8];
    uint16_t bytes_per_sector;
    uint8_t sect_per_cluster;
    uint16_t reserved_sectors;
    uint8_t number_of_fat;
    uint16_t root_entries;
    uint16_t total_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t heads_per_cylinder;
    uint32_t hidden_sectors;
    uint32_t total_sectors_big;
} __attribute__((packed));

struct fat_header_extended {
    uint8_t drive_no;
    uint8_t win_nt;
    uint8_t ext_boot_signature;
    uint32_t serial_number;
    uint8_t volume_label[11];
    uint8_t file_system[8];
} __attribute__((packed));

struct fat_h {
    struct fat_header primary;
    union fat_h_ext {
        struct fat_header_extended ext;
    } shared;
};

struct fat_dir_item {
    uint8_t name[8];
    uint8_t ext[3];
    uint8_t attribute;
    uint8_t reserved;
    uint8_t create_time_decisecond;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t last_access;
    uint16_t high_first_cluster;
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint16_t low_first_cluster;
    uint32_t file_size;
} __attribute__((packed));

struct fat_dir {
    struct fat_dir_item* item;
    int total;
    int sector_pos;
    int sector_end;
};

struct fat_item {
    union {
        struct fat_dir_item* item;
        struct fat_dir* dir;
    };
    FAT_ITEM_TYPE type;
};

struct fat_item_descriptor {
    struct fat_item* item;
    uint32_t pos;
};

struct fat_private {
    struct fat_h header;
    struct fat_dir root;
    struct disk_stream* cluster_read;
    struct disk_stream* fat_read;
    struct disk_stream* dir_read;
};


int fat16_resolve(struct disk* disk);
void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode);

struct file_system fat16_fs = {
    .resolve = fat16_resolve,
    .open = fat16_open
};

struct file_system* fat16_init() {
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}

// setup private FAT structure
static void fat16_init_private(struct disk* disk, struct fat_private* private) {
    memset(private, 0, sizeof(struct fat_private));
    private->cluster_read = disk_stream_new(disk->id);
    private->fat_read = disk_stream_new(disk->id);
    private->dir_read = disk_stream_new(disk->id);
}

// convert sector to absolute byte index
int fat16_sector_to_abs(struct disk* disk, int sector) {
    return sector * disk->sector_size;
}

//
int fat16_get_dir_items(struct disk* disk, uint32_t dir_sector_start) {
    struct fat_dir_item item;
    struct fat_dir_item empty_item;
    memset(&empty_item, 0, sizeof(empty_item));

    struct fat_private* fat_private = disk->fs_private;
    int status = 0;
    int dir_start = dir_sector_start * disk->sector_size;
    struct disk_stream* stream = fat_private->dir_read;
    int i = 0;

    if (disk_stream_seek(stream, dir_start) != OK) {
        status = -EIO;
        goto out;
    }
    while (1) {
        if (disk_stream_read(stream, &item, sizeof(item)) != OK) {
            status = -EIO;
            goto out;
        }
        if (item.name[0] == 0x00) {
            break;
        }
        if (item.name[0] == 0xE5) {
            continue;  // directory entry is free (available)
        }
        i++;
    }
    status = i;
out:
    return status;
}

int fat16_get_root(struct disk* disk, struct fat_private* fat_private, struct fat_dir* dir_out) {
    int status = 0;
    struct fat_header* primary = &fat_private->header.primary;
    int root_pos = (primary->number_of_fat * primary->sectors_per_fat) + primary->reserved_sectors;
    int root_entries = fat_private->header.primary.root_entries;
    int root_size = (root_entries * sizeof(struct fat_dir_item));
    int total_sectors = root_size / disk->sector_size;

    if (root_size % disk->sector_size) {
        total_sectors++;
    }
    int total_items = fat16_get_dir_items(disk, root_pos);
    
    struct fat_dir_item* dir = kzalloc(root_size);
    if (!dir) {
        status = -ENOMEM;
        goto out;
    }
    struct disk_stream* stream = fat_private->dir_read;
    if (disk_stream_seek(stream, fat16_sector_to_abs(disk, root_pos)) != OK) {
        status = -EIO;
        goto out;
    }
    if (disk_stream_read(stream, dir, root_size) != OK) {
        status = -EIO;
        goto out;
    }

    dir_out->item = dir;
    dir_out->total = total_items;
    dir_out->sector_pos = root_pos;
    dir_out->sector_end = root_pos + (root_size / disk->sector_size);
out:
    return status;
}

int fat16_resolve(struct disk* disk) {
    int status = OK;
    struct fat_private* fat_private = kzalloc(sizeof(struct fat_private));
    
    fat16_init_private(disk, fat_private);
    disk->fs_private = fat_private;
    disk->fs = &fat16_fs;

    struct disk_stream* stream = disk_stream_new(disk->id);
    if (!stream) {
        status = -ENOMEM;
        goto out;
    }
    if (disk_stream_read(stream, &fat_private->header, sizeof(fat_private->header)) != OK) {
        status = -EIO;
        goto out;
    }

    if (fat_private->header.shared.ext.ext_boot_signature != 0x29) {
        status = -EFSNOTUS;
        goto out;
    }
    if (fat16_get_root(disk, fat_private, &fat_private->root) != OK) {
        status = -EIO;
        goto out;
    }
out:
    if (stream) {
        disk_stream_close(stream);
    }
    if (status < 0) {
        kfree(fat_private);
        disk->fs_private = 0;
    }
    return status;
}

void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode) {
    return 0;
}
