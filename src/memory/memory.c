#include "memory.h"

void* memset(void* ptr, int c, size_t size) {
    char* c_ptr = (char*) ptr;
    
    for (int i = 0; i < size; i++) {
        c_ptr[i] = (char) c;
    }
    return ptr;
}

int memcmp(void* ptr1, void* ptr2, int size) {
    char* c1 = ptr1;
    char* c2 = ptr2;
    while (size-- > 0) {
        if (*c1++ != *c2++) {
            return c1[-1] > c2[-1] ? -1 : 1;
        }
    }
    return 0;
}
