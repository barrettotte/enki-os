#include "string.h"

int strlen(const char* s) {
    int i = 0;
    while (s[i]) {
        i++;
    }
    return i;
}

int strnlen(const char* s, int n) {
    int i = 0;
    for (i = 0; i < n; i++) {
        if (!s[i]) {
            break;
        }
    }
    return i;
}

bool is_digit(char c) {
    return c >= 48 && c <= 57;
}

int to_digit(char c) {
    return c - 48;
}
