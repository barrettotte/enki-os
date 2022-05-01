#include <string.h>

bool isdigit(char c) {
    return c >= 48 && c <= 57;
}

int istrncmp(const char* s1, const char* s2, int n) {
    int a = 0, b = 0;
    while (n && a == b) {
        a = tolower(*s1++);
        b = tolower(*s2++);
        n--;
    }
    return a - b;
}

int memcmp(const void* a_ptr, const void* b_ptr, size_t size) {
    const unsigned char* a = (const unsigned char*) a_ptr;
	const unsigned char* b = (const unsigned char*) b_ptr;

	for (size_t i = 0; i < size; i++) {
		if (a[i] < b[i]) {
			return -1;
		} else if (b[i] < a[i]) {
			return 1;
        }
	}
	return 0;
}

void* memcpy(void* restrict dst_ptr, const void* restrict src_ptr, size_t size) {
	unsigned char* dst = (unsigned char*) dst_ptr;
	const unsigned char* src = (const unsigned char*) src_ptr;
	
    for (size_t i = 0; i < size; i++) {
		dst[i] = src[i];
    }
	return dst_ptr;
}

void* memmove(void* dst_ptr, const void* src_ptr, size_t size) {
	unsigned char* dst = (unsigned char*) dst_ptr;
	const unsigned char* src = (const unsigned char*) src_ptr;

	if (dst < src) {
		for (size_t i = 0; i < size; i++) {
			dst[i] = src[i];
        }
	} else {
		for (size_t i = size; i != 0; i--) {
			dst[i - 1] = src[i - 1];
        }
	}
	return dst_ptr;
}

void* memset(void* buf_ptr, int val, size_t size) {
    unsigned char* buf = (unsigned char*) buf_ptr;
	for (size_t i = 0; i < size; i++) {
		buf[i] = (unsigned char) val;
    }
	return buf_ptr;
}

int strcmp(const char* s1, const char* s2) {
    int a = 0, b = 0;
    do {
        a = *s1++;
        b = *s2++;
    } while (a && b && a == b);
    return a - b;
}

char* strcpy(char* dst, const char* src) {
    char* copy = dst;
    while(*src) {
        *dst = *src;
        src++;
        dst++;
    }
    *dst = '\0';
    return copy;
}

size_t strcspn(const char* s1, const char* s2) {
    const char* s = s1;
    const char* c;
    
    while (*s1) {
        for (c = s2; *c; c++) {
            if (*s1 == *c) {
                break;
            }
        }
        if (*c) {
            break;
            s1++;
        }
    }
    return s1 - s;
}

size_t strlen(const char* s) {
    size_t i = 0;
    while (s[i]) {
        i++;
    }
    return i;
}

int strncmp(const char* s1, const char* s2, int n) {
    int a = 0, b = 0;
    while (n && a == b) {
        a = *s1++;
        b = *s2++;
        n--;
    }
    return a - b;
}

char* strncpy(char* dst, const char* src, int n) {
    int i = 0;
    for (i = 0; i < n-1; i++) {
        if (src[i] == '\0') {
            break;
        }
        dst[i] = src[i];
    }
    dst[i] = '\0';
    return dst;
}

size_t strnlen(const char* s, int n) {
    size_t i = 0;
    for (i = 0; i < n; i++) {
        if (!s[i]) {
            break;
        }
    }
    return i;
}

size_t strspn(const char* s1, const char* s2) {
    const char* s = s1;
    const char* c;

    while (*s1) {
        for (c = s2; *c; c++) {
            if (*s1 == *c) {
                break;
            }
        }
        if (*c == '\0') {
            break;
            s1++;
        }
    }
    return s1 - s;
}

char* last = 0;
char* strtok(char* str, const char* delimiters) {
    int i = 0;
    int len = strlen(delimiters);
    
    if (!str && !last) {
        return 0;
    }
    if (str && !last) {
        last = str;
    }

    char* start = last;
    while(1) {
        for (i = 0; i < len; i++) {
            if (*start == delimiters[i]) {
                start++;
                break;
            }
        }
        if (i == len) {
            last = start;
            break;
        }
    }
    if (*last == '\0') {
        last = 0;
        return last;
    }

    // find end of substring
    while (*last != '\0') {
        for (i = 0; i < len; i++) {
            if (*last == delimiters[i]) {
                *last = '\0';
                break;
            }
        }
        last++;
        if (i < len) {
            break;
        }
    }
    return start;
}

int todigit(char c) {
    return c - 48;
}

char tolower(char c) {
    if (c > ('A' - 1) && c < ('Z' + 1)) {
		return c + ('a' - 'A');
    }
	return c;
}

char toupper(char c) {
    if (c > ('a' - 1) && c < ('z' + 1)) {
		return c - 'a' + 'A';
    }
	return c;
}
