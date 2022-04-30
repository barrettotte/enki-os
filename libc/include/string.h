#ifndef STRING_H
#define STRING_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// fill block of memory with value
void* memset(void* buf_ptr, int val, size_t size);

// compare two blocks of memory
int memcmp(const void* a_ptr, const void* b_ptr, size_t size);

// copy memory from one location to another
void* memcpy(void* restrict dst_ptr, const void* restrict src_ptr, size_t size);

// move memory from one location to another
void* memmove(void* dst_ptr, const void* src_ptr, size_t size);

// calculate length of string
size_t strlen(const char* s);

// calculate length of string up to n
size_t strnlen(const char* s, int n);

// compare two strings
int strcmp(const char* s1, const char* s2);

// compare two strings insensitive up to given length
int istrncmp(const char* s1, const char* s2, int n);

// compare two strings up to given length
int strncmp(const char* s1, const char* s2, int n);

// copy string from source to destination
char* strcpy(char* dst, const char* src);

// copy string from source to destination up to n
char* strncpy(char* dst, const char* src, int n);

// tokenize a string 
char* strtok(char* str, const char* delimiters);

// check if character is a digit
bool isdigit(char c);

// convert character to digit (assumes character can be converted to digit)
int todigit(char c);

// convert character to lowercase
char tolower(char c);

// convert character to uppercase
char toupper(char c);

#ifdef __cplusplus
}
#endif

#endif