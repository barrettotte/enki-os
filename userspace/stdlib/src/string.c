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

int strnlen_until(const char* s, int n, char terminator) {
    int i = 0;
    for (i = 0; i < n; i++) {
        if (s[i] == '\0' || s[i] == terminator) {
            break;
        }
    }
    return i;
}

int istrncmp(const char* s1, const char* s2, int n) {
    unsigned char c1, c2;
    while (n-- > 0) {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 != c2 && tolower(c1) != tolower(c2)) {
            return c1 - c2;
        }
        if (c1 == '\0') {
            return 0;
        }
    }
    return 0;
}

int strncmp(const char* s1, const char* s2, int n) {
    unsigned char c1, c2;
    while (n-- > 0) {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 != c2) {
            return c1 - c2;
        }
        if (c1 == '\0') {
            return 0;
        }
    }
    return 0;
}

char* strcpy(char* dest, const char* src) {
    char* copy = dest;
    while(*src) {
        *dest = *src;
        src++;
        dest++;
    }
    *dest = '\0';
    return copy;
}

char* strncpy(char* dest, const char* src, int n) {
    int i = 0;
    for (i = 0; i < n-1; i++) {
        if (src[i] == '\0') {
            break;
        }
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return dest;
}

bool isdigit(char c) {
    return c >= 48 && c <= 57;
}

int todigit(char c) {
    return c - 48;
}

char tolower(char c) {
    if (c >= 65 && c <= 90) {
        c += 32;
    }
    return c;
}

char* sp = 0;

char* strtok(char* str, const char* delimiters) {
    int i = 0;
    int len = strlen(delimiters);
    if (!str && !sp) {
        return 0;
    }

    if (str && !sp) {
        sp = str;
    }

    char* p_start = sp;
    while(1) {
        for (i = 0; i < len; i++) {
            if (*p_start == delimiters[i]) {
                p_start++;
                break;
            }
        }
        if (i == len) {
            sp = p_start;
            break;
        }
    }
    if (*sp == '\0') {
        sp = 0;
        return sp;
    }

    // find end of substring
    while (*sp != '\0') {
        for (i = 0; i < len; i++) {
            if (*sp == delimiters[i]) {
                *sp = '\0';
                break;
            }
        }
        sp++;
        if (i < len) {
            break;
        }
    }

    return p_start;
}