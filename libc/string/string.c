#include <string.h>

void* memset(void* buf_ptr, int val, size_t size) {
    unsigned char* buf = (unsigned char*) buf_ptr;
	for (size_t i = 0; i < size; i++) {
		buf[i] = (unsigned char) val;
    }
	return buf_ptr;
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

size_t strlen(const char* s) {
    size_t i = 0;
    while (s[i]) {
        i++;
    }
    return i;
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

char* strtok(char* str, const char* delimiters) {
    char* sp = 0;
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

bool isdigit(char c) {
    return c >= 48 && c <= 57;
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
