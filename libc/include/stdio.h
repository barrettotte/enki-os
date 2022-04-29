#ifndef STDIO_H
#define STDIO_H

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char* msg, ...);
int putchar(int);
int puts(const char*);

#ifdef __cplusplus
}
#endif

#endif