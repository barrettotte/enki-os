#ifndef STDIO_H
#define STDIO_H

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

//
int putchar(int c);

//
int printf(const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif