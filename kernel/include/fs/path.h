#ifndef PATH_H
#define PATH_H

struct path_root {
    int drive_no;
    struct path_part* first;
};

struct path_part {
    const char* part;
    struct path_part* next;
};

// free allocated memory given to file path root
void path_free(struct path_root* root);

// parse file path
struct path_root* path_parse(const char* path, const char* curr_dir);

#endif