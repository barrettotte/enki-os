#ifndef KERNEL_H
#define KERNEL_H

// switch to kernel page directory and kernel data segment
void kernel_page();

// switch to kernel data segment
void kernel_registers();

// kernel entry
void kernel_main();

#endif