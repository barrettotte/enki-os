#include <disk/disk.h>
#include <disk/disk_stream.h>
#include <fs/fat16.h>
#include <kernel/config.h>
#include <kernel/status.h>
#include <memory/kheap.h>
#include <stdint.h>
#include <string.h>

#define ENKI_FAT16_SIGNATURE  0x29
#define ENKI_FAT16_ENTRY_SIZE 0x02
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

struct fat_file_descriptor {
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
int fat16_read(struct disk* disk, void* desc, uint32_t size, uint32_t blocks, char* out);
int fat16_close(void *private);
int fat16_seek(void* private, uint32_t offset, FILE_SEEK_MODE mode);
int fat16_stat(struct disk* disk, void* private, struct file_stat* stat);

struct file_system fat16_fs = {
    .resolve = fat16_resolve,
    .open = fat16_open,
    .read = fat16_read,
    .close = fat16_close,
    .seek = fat16_seek,
    .stat = fat16_stat
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
    int dir_start = dir_sector_start * disk->sector_size;
    struct disk_stream* stream = fat_private->dir_read;
    int i = 0;

    if (disk_stream_seek(stream, dir_start)) {
        return -EIO;
    }
    while (1) {
        if (disk_stream_read(stream, &item, sizeof(item))) {
            return -EIO;
        } else if (item.name[0] == 0x00) {
            break;
        } else if (item.name[0] == 0xE5) {
            continue;  // directory entry is free (available)
        }
        i++;
    }
    return i;
}

int fat16_get_root(struct disk* disk, struct fat_private* fat_private, struct fat_dir* dir_out) {
    struct fat_dir_item* dir = 0;
    struct fat_header* primary = &fat_private->header.primary;

    int root_pos = (primary->number_of_fat * primary->sectors_per_fat) + primary->reserved_sectors;
    int root_entries = fat_private->header.primary.root_entries;
    int root_size = (root_entries * sizeof(struct fat_dir_item));
    int total_sectors = root_size / disk->sector_size;

    if (root_size % disk->sector_size) {
        total_sectors++;
    }
    int total_items = fat16_get_dir_items(disk, root_pos);
    
    dir = kzalloc(root_size);
    if (!dir) {
        return -ENOMEM;
    }

    struct disk_stream* stream = fat_private->dir_read;
    if (disk_stream_seek(stream, fat16_sector_to_abs(disk, root_pos))) {
        kfree(dir);
        return -EIO;
    }
    if (disk_stream_read(stream, dir, root_size)) {
        kfree(dir);
        return -EIO;
    }

    dir_out->item = dir;
    dir_out->total = total_items;
    dir_out->sector_pos = root_pos;
    dir_out->sector_end = root_pos + (root_size / disk->sector_size);
    return 0;
}

int fat16_resolve(struct disk* disk) {
    struct fat_private* fat_private = kzalloc(sizeof(struct fat_private));
    fat16_init_private(disk, fat_private);
    disk->fs_private = fat_private;
    disk->fs = &fat16_fs;

    struct disk_stream* stream = disk_stream_new(disk->id);
    if (!stream) {
        kfree(fat_private);
        disk->fs_private = 0;
        return -ENOMEM; // failed to allocate memory
    }
    if (disk_stream_read(stream, &fat_private->header, sizeof(fat_private->header))) {
        disk_stream_close(stream);
        kfree(fat_private);
        disk->fs_private = 0;
        return -EIO;
    }
    if (fat_private->header.shared.ext.ext_boot_signature != 0x29) {
        disk_stream_close(stream);
        kfree(fat_private);
        disk->fs_private = 0;
        return -EFSNOTUS;  // OS isn't in charge of this
    }
    if (fat16_get_root(disk, fat_private, &fat_private->root)) {
        disk_stream_close(stream);
        kfree(fat_private);
        disk->fs_private = 0;
        return -EIO;
    }
    return 0;
}

// get file name without padding
void fat16_fmt_str(char** out, const char* in, size_t size) {
    int i = 0;
    while (*in != 0x00 && *in != 0x20 && i < size) {
        **out = *in;
        *out += 1;
        in++;
        i++;
    }
    **out = 0x00;
}

// resolve file name with extension.  ex:  "TEST    TXT" => "test.txt"
void fat16_file_name_resolve(struct fat_dir_item* item, char* out, int max) {
    memset(out, 0x00, max);
    char* tmp = out;
    fat16_fmt_str(&tmp, (const char*) item->name, sizeof(item->name));
    
    if (item->ext[0] != 0x00 && item->ext[0] != 0x20) {
        *tmp++ = '.';
        fat16_fmt_str(&tmp, (const char*) item->ext, sizeof(item->ext));
    }
}

// clone directory item
struct fat_dir_item* fat16_clone_dir_item(struct fat_dir_item* item, int size) {
    if (size < sizeof(struct fat_dir_item)) {
        return 0;
    }
    struct fat_dir_item* copy = kzalloc(size);
    if (!copy) {
        return 0;
    }
    memcpy(copy, item, size);
    return copy;
}

// 
static uint32_t fat16_get_first_fat_sector(struct fat_private* private) {
    return private->header.primary.reserved_sectors;
}

// get FAT entry from FAT
static int fat16_get_fat_entry(struct disk* disk, int cluster) {
    int result = 0;
    struct fat_private* private = disk->fs_private;
    struct disk_stream* stream = private->fat_read;
    if (!stream) {
        return 0;
    }
    
    uint32_t fat_table_pos = fat16_get_first_fat_sector(private) * disk->sector_size;
    result = disk_stream_seek(stream, fat_table_pos + (cluster * ENKI_FAT16_ENTRY_SIZE));
    if (result < 0) {
        return result;
    }

    uint16_t read_result = 0;
    result = disk_stream_read(stream, &read_result, sizeof(read_result));
    if (result < 0) {
        return result;
    }
    return read_result;
}

// get cluster to use based on starting cluster and offset
static int fat16_get_cluster_offset(struct disk* disk, int cluster_start, int offset) {
    struct fat_private* private = disk->fs_private;
    int cluster_size = private->header.primary.sect_per_cluster * disk->sector_size;
    int cluster_target = cluster_start;
    int ahead = offset / cluster_size;

    for (int i = 0; i < ahead; i++) {
        int entry = fat16_get_fat_entry(disk, cluster_target);
        switch (entry) {
            case 0x00:   // FAT is corrupted
            case 0xFF0:  // reserved sector
            case 0xFF6:  // reserved sector
            case 0xFF7:  // bad sector
            case 0xFF8:  // end of file
            case 0xFFF:  // error
                return -EIO;
        }
        cluster_target = entry;
    }
    return cluster_target;
}

// get 32-bit address of first cluster
static uint32_t fat16_get_first_cluster(struct fat_dir_item* item) {
    return (item->high_first_cluster) | (item->low_first_cluster);
}

// convert cluster number to sector
static int fat16_cluster_to_sector(struct fat_private* private, int cluster) {
    return private->root.sector_end + ((cluster - 2) * private->header.primary.sect_per_cluster);
}

//
static int fat16_read_internal_stream(struct disk* disk, struct disk_stream* stream, int cluster, int offset, int total, void* out) {
    int result = 0;
    struct fat_private* private = disk->fs_private;
    int cluster_size = private->header.primary.sect_per_cluster * disk->sector_size;
    
    int cluster_target = fat16_get_cluster_offset(disk, cluster, offset);
    if (cluster_target < 0) {
        return cluster_target;
    }

    int cluster_offset = offset % cluster_size;
    int start_sector = fat16_cluster_to_sector(private, cluster_target);
    int start_pos = (start_sector * disk->sector_size) + cluster_offset;
    int to_read = total > cluster_size ? cluster_size : total;
    
    result = disk_stream_seek(stream, start_pos);
    if (result) {
        return result;
    }
    result = disk_stream_read(stream, out, to_read);
    if (result) {
        return result;
    }
    
    total -= to_read;
    if (total > 0) {
        result = fat16_read_internal_stream(disk, stream, cluster, offset + to_read, total, out + to_read);
    }

    return result;
}

// read from disk via a series of clusters
static int fat16_read_internal(struct disk* disk, int start_cluster, int offset, int total, void* out) {
    struct fat_private* fs_private = disk->fs_private;
    struct disk_stream* stream = fs_private->cluster_read;
    return fat16_read_internal_stream(disk, stream, start_cluster, offset, total, out);
}

// free allocated memory of FAT directory
void fat16_free_dir(struct fat_dir* dir) {
    if (dir) {
        if (!dir->item) {
            kfree(dir->item);
        }
        kfree(dir);
    }
}

// free allocated memory of FAT item
void fat16_fat_item_free(struct fat_item* item) {
    if (item->type == FAT_ITEM_TYPE_DIR) {
        fat16_free_dir(item->dir);
    } else if (item->type == FAT_ITEM_TYPE_FILE) {
        kfree(item->item);
    } else {
        // TODO: panic!
    }
    kfree(item);
}

//
struct fat_dir* fat16_load_fat_dir(struct disk* disk, struct fat_dir_item* item) {
    struct fat_dir* dir = 0;
    struct fat_private* private = disk->fs_private;
    
    if (!(item->attribute & FAT_FILE_SUBDIRECTORY)) {
        return 0;
    }
    dir = kzalloc(sizeof(struct fat_dir));
    if (!dir) {
        return 0;
    }

    int cluster = fat16_get_first_cluster(item);
    int sector = fat16_cluster_to_sector(private, cluster);
    int total = fat16_get_dir_items(disk, sector);
    dir->total = total;

    int size = dir->total * sizeof(struct fat_dir_item);
    dir->item = kzalloc(size);
    if (dir->item) {
        fat16_free_dir(dir);
        return 0;
    }
    if (fat16_read_internal(disk, cluster, 0x00, size, dir->item)) {
        fat16_free_dir(dir);
    }
    return dir;
}

//
struct fat_item* fat16_new_item_for_dir(struct disk* disk, struct fat_dir_item* item) {
    struct fat_item* f = kzalloc(sizeof(struct fat_item));
    if (!f) {
        return 0;  // couldn't allocate
    }

    if (item->attribute & FAT_FILE_SUBDIRECTORY) {
        f->dir = fat16_load_fat_dir(disk, item);
        f->type = FAT_ITEM_TYPE_DIR;
    } else {
        f->type = FAT_ITEM_TYPE_FILE;
        f->item = fat16_clone_dir_item(item, sizeof(struct fat_dir_item));
    }
    return f;
}

// find file/directory in directory using path
struct fat_item* fat16_find_item_in_dir(struct disk* disk, struct fat_dir* dir, const char* name) {
    struct fat_item* item = 0;
    char tmp_name[ENKI_MAX_PATH];

    for (int i = 0; i < dir->total; i++) {
        fat16_file_name_resolve(&dir->item[i], tmp_name, sizeof(tmp_name));
        
        if (istrncmp(tmp_name, name, sizeof(tmp_name)) == 0) {
            item = fat16_new_item_for_dir(disk, &dir->item[i]);
        }
    }
    return item;
}

//
struct fat_item* fat16_get_dir_entry(struct disk* disk, struct path_part* path) {
    struct fat_private* fat_private = disk->fs_private;
    struct fat_item* root = fat16_find_item_in_dir(disk, &fat_private->root, path->part);

    if (!root) {
        return 0;  // item not found
    }
    struct path_part* next_path = path->next;
    struct fat_item* current = root;
    while (next_path != 0) {
        if (current->type != FAT_ITEM_TYPE_DIR) {
            return 0;  // can't access file as a directory
        }
        struct fat_item* tmp = fat16_find_item_in_dir(disk, current->dir, next_path->part);
        fat16_fat_item_free(current);
        current = tmp;
        next_path = next_path->next;
    }
    return current;
}

void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode) {
    if (mode != FILE_MODE_READ) {
        return (void*) -ERDONLY;
    }

    struct fat_file_descriptor* descriptor = kzalloc(sizeof(struct fat_file_descriptor));
    if (!descriptor) {
        return (void*) -ENOMEM;
    }
    descriptor->item = fat16_get_dir_entry(disk, path);
    if (!descriptor->item) {
        kfree(descriptor);
        return (void*) -EIO;  // couldn't find file
    }
    descriptor->pos = 0;
    return descriptor;
}

int fat16_read(struct disk* disk, void* desc, uint32_t size, uint32_t blocks, char* out) {
    int result = 0;
    struct fat_file_descriptor* fat_desc = desc;
    struct fat_dir_item* item = fat_desc->item->item;
    int offset = fat_desc->pos;

    for (uint32_t i = 0; i < blocks; i++) {
        result = fat16_read_internal(disk, fat16_get_first_cluster(item), offset, size, out);
        if (result < 0) {
            return result;
        }
        out += size;
        offset += size;
    }
    return blocks;
}

static void fat16_free_file_descriptor(struct fat_file_descriptor* desc) {
    fat16_fat_item_free(desc->item);
    kfree(desc);
}

int fat16_close(void* private) {
    fat16_free_file_descriptor((struct fat_file_descriptor*) private);
    return 0;
}

int fat16_seek(void* private, uint32_t offset, FILE_SEEK_MODE mode) {
    struct fat_file_descriptor* desc = private;
    struct fat_item* desc_item = desc->item;
    if (desc_item->type != FAT_ITEM_TYPE_FILE) {
        return -EINVARG; // only seek on files
    }

    struct fat_dir_item* item = desc_item->item;
    if (offset >= item->file_size) {
        return -EIO;
    }
    switch (mode) {
        case SEEK_SET:
            desc->pos = offset;
            break;
        case SEEK_CUR:
            desc->pos += offset;
            break;
        case SEEK_END:
            return -EUNIMP;
        default:
            return -EINVARG;
    }
    return 0;    
}

int fat16_stat(struct disk* disk, void* private, struct file_stat* stat) {
    struct fat_file_descriptor* desc = (struct fat_file_descriptor*) private;
    struct fat_item* desc_item = desc->item;

    if (desc->item->type != FAT_ITEM_TYPE_FILE) {
        return -EINVARG;
    }
    struct fat_dir_item* item = desc_item->item;
    stat->file_size = item->file_size;
    stat->flags = 0x00;

    if (item->attribute & FAT_FILE_READ_ONLY) {
        stat->flags |= FILE_STAT_READ_ONLY;
    }
    return 0;
}
