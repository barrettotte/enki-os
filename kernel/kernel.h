#ifndef KERNEL_H
#define KERNEL_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

#define ERROR(val) (void *)(val)
#define ERROR_I(val) (int)(val)
#define IS_ERR(val) ((int) val < 0)

// write character to current terminal position
void tty_writechar(char c, char color);

// halt kernel and display panic message
void panic(const char* msg);

// switch to kernel page directory and kernel data segment
void kernel_page();

// kernel entry
void kernel_main();

// print string to terminal
void print(const char* s);

// switch to kernel data segment
void kernel_registers();

#endif