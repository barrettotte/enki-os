#include "enki.h"

void* malloc(size_t size) {
    return enki_malloc(size);
}

void free(void* ptr) {
    enki_free(ptr);
}

char* itoa(int i) {
    static char s[12];
    int idx = 11;
    s[11] = 0;
    char neg = 1;

    if (i >= 0) {
        neg = 0;
        i = -i;
    }

    while (i) {
        s[--idx] = '0' - (i % 10);
        i /= 10;
    }

    if (idx == 11) {
        s[--idx] = '0';
    }
    if (neg) {
        s[--idx] = '-';
    }
    return &s[idx];
}