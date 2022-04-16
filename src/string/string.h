#ifndef STRING_H
#define STRING_H

#include <stdbool.h>

// calculate length of string
int strlen(const char* s);

// calculate length of string up to n
int strnlen(const char* s, int n);

// read string until provided terminator or null terminator
int strnlen_until(const char* s, int n, char terminator);

// compare two strings insensitive
int istrncmp(const char* s1, const char* s2, int n);

// compare two strings
int strncmp(const char* s1, const char* s2, int n);

// copy string from source to destination
char* strcpy(char* dest, const char* src);

// copy string from source to destination up to n
char* strncpy(char* dest, const char* src, int n);

// check if character is a digit
bool isdigit(char c);

// convert character to digit (assumes character can be converted to digit)
int todigit(char c);

// convert character to lowercase
char tolower(char c);

#endif