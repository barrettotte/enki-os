#ifndef STRING_H
#define STRING_H

#include <stdbool.h>

// calculate length of string
int strlen(const char* s);

// calculate length of string up to n
int strnlen(const char* s, int n);

// check if character is a digit
bool is_digit(char c);

// convert character to digit (assumes character can be converted to digit)
int to_digit(char c);

#endif