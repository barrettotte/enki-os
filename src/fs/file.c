#include "../config.h"
#include "../kernel.h"
#include "../status.h"
#include "../disk/disk.h"
#include "../memory/heap/kheap.h"
#include "../string/string.h"
#include "fat/fat16.h"
#include "file.h"

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

// 
static void file_free_descriptor(struct file_descriptor* desc) {
    file_descriptors[desc->index-1] = 0x00;
    kfree(desc);
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

// load file systems built into the kernel
static void fs_static_load() {
    fs_insert_file_system(fat16_init());
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

void fs_load() {
    memset(file_systems, 0, sizeof(file_systems));
    fs_static_load();
}

void fs_init() {
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
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

// convert string to valid file mode
FILE_MODE file_get_mode_from_str(const char* s) {
    FILE_MODE mode = FILE_MODE_INVALID;
    if (strncmp(s, "r", 1) == 0) {
        mode = FILE_MODE_READ;
    } else if (strncmp(s, "w", 1) == 0) {
        mode = FILE_MODE_WRITE;
    } else if (strncmp(s, "a", 1) == 0) {
        mode = FILE_MODE_APPEND;
    }
    return mode;
}

int fopen(const char* file_name, const char* mode) {
    int status = 0;

    struct path_root* root = path_parse(file_name, NULL);
    if (!root) {
        status = -EINVARG;
        goto out;
    }
    if (!root->first) {
        status = -EINVARG; // i:/
        goto out;
    }
    
    struct disk* disk = disk_get(root->drive_no);
    if (!disk) {
        status = -EIO;
        goto out; // drive doesn't exist
    }
    if (!disk->fs) {
        status = -EIO;
        goto out;
    }

    FILE_MODE fmode = file_get_mode_from_str(mode);
    if (fmode == FILE_MODE_INVALID) {
        status = -EINVARG;
        goto out;  // invalid file mode
    }

    void* desc_private = disk->fs->open(disk, root->first, fmode);
    if (IS_ERR(desc_private)) {
        status = ERROR_I(desc_private);
        goto out;
    }

    struct file_descriptor* desc = 0;
    status = file_new_descriptor(&desc);
    if (status < 0) {
        goto out;
    }
    desc->fs = disk->fs;
    desc->private_data = desc_private;
    desc->disk = disk;
    status = desc->index;
out:
    if (status < 0) {
        status = 0;
    }
    return status;
}

int fread(void* buf, uint32_t size, uint32_t blocks, int fd) {
    int status = 0;
    if (size == 0 || blocks == 0 || fd < 1) {
        status = -EINVARG;
        goto out;
    }

    struct file_descriptor* desc = file_get_descriptor(fd);
    if (!desc) {
        status = -EINVARG;
        goto out;
    }

    status = desc->fs->read(desc->disk, desc->private_data, size, blocks, (char *) buf);

out:
    return status;
}

int fclose(int fd) {
    int result = 0;
    struct file_descriptor* desc = file_get_descriptor(fd);
    if (!desc) {
        result = -EIO;
        goto out;
    }

    result = desc->fs->close(desc->private_data);
    if (result == OK) {
        file_free_descriptor(desc);
    }
out:
    return result;
}

int fseek(int fd, int offset, FILE_SEEK_MODE mode) {
    int result = 0;
    struct file_descriptor* desc = file_get_descriptor(fd);
    if (!desc) {
        result = -EIO;
        goto out;
    }
    result = desc->fs->seek(desc->private_data, offset, mode);
out:
    return result;
}

int fstat(int fd, struct file_stat* stat) {
    int result = 0;
    struct file_descriptor* desc = file_get_descriptor(fd);
    if (!desc) {
        result = -EIO;
        goto out;
    }
    result = desc->fs->stat(desc->disk, desc->private_data, stat);
out:
    return result;
}
