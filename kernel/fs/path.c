#include "../include/kernel/config.h"
#include "../include/kernel/status.h"
#include "../memory/heap/kheap.h"
#include "../string/string.h"
#include "path.h"

// check if file path is valid; only three levels => 0:/dir/file.ext
static int path_is_valid(const char* path) {
    int len = strnlen(path, ENKI_MAX_PATH);
    return (len >= 3 && isdigit(path[0]) && memcmp((void *) &path[1], ":/", 2) == 0);
}

// get drive number from file path
static int path_get_drive(const char** path) {
    if (!path_is_valid(*path)) {
        return -EBADPATH;
    }
    int drive_no = todigit(*path[0]);
    *path += 3; // consume drive number "0:/"
    return drive_no;
}

// create root of file path
static struct path_root* path_create_root(int drive_no) {
    struct path_root* root = kzalloc(sizeof(struct path_root));
    root->drive_no = drive_no;
    root->first = 0;
    return root;
}

// get a part of a file path
static const char* path_get_part(const char** path) {
    char* part = kzalloc(ENKI_MAX_PATH);
    int i = 0;
    while (**path != '/' && **path != 0x00) {
        part[i] = **path;
        *path += 1;
        i++;
    }
    if (**path == '/') {
        *path += 1;  // consume possible repeated path separator
    }

    // clean up if nothing was parsed
    if (i == 0) {
        kfree(part);
        part = 0;
    }
    return part;
}

// parse a path part from file path
struct path_part* path_parse_part(struct path_part* last_part, const char** path) {
    const char* part_str = path_get_part(path);
    if (!part_str) {
        return 0;
    }

    struct path_part* part = kzalloc(sizeof(struct path_part));
    part->part = part_str;
    part->next = 0x00;

    if (last_part) {
        last_part->next = part;
    }
    return part;
}

void path_free(struct path_root* root) {
    struct path_part* current = root->first;
    while (current) {
        struct path_part* next = current->next;
        kfree((void *) current->part);
        kfree(current);
        current = next;
    }
    kfree(root);
}

struct path_root* path_parse(const char* path, const char* curr_dir) {
    int result = 0;
    const char* tmp_path = path;
    struct path_root* root = 0;

    if (strlen(path) > ENKI_MAX_PATH) {
        goto out;
    }
    
    result = path_get_drive(&tmp_path);
    if (result < 0) {
        goto out;
    }

    root = path_create_root(result);
    if (!root) {
        goto out;
    }

    struct path_part* first = path_parse_part(NULL, &tmp_path);
    if (!first) {
        goto out;
    }
    root->first = first;

    struct path_part* part = path_parse_part(first, &tmp_path);
    while (part) {
        part = path_parse_part(part, &tmp_path);
    }

out:
    return root;
}
