#ifndef KERNEL_H
#define KERNEL_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

#define ERROR(val) (void *)(val)
#define ERROR_I(val) (int)(val)
#define IS_ERR(val) ((int) val < 0)

// kernel entry
void kernel_main();

// print string to terminal
void print(const char* s);

#endif