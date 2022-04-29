# enki-os

A basic x86 kernel to learn how operating systems get from BIOS to userland.

This is my first dive into the deep world of operating systems. I hope to make 
another kernel in the future using the things I learned here.

## Features / Goals

- [x] Custom bootloader
- [x] FAT-16 file system (except `fwrite`)
- [x] Keyboard driver
- [x] Static ELF file loading
- [x] Partial LibC
- [x] Userland with basic shell
- [x] Basic Multitasking (timer based)

## Building Locally

- Install dependencies and build cross compiler: `make toolchain`
- Build OS image: `make`
- Build OS image and launch in QEMU: `make qemu`
- Build OS image and launch with GDB connected to QEMU: `make debug`

## TODO Items

- TODO: comments on all C functions and asm
- TODO: move `tty_*` functions out of `kernel.c`
- TODO: reevaluate use of `goto`
- TODO: refactor some paths to `libc`
- TODO: rename `isr_80h` to `syscall`?
  - move `idt.c` `isr_80h` commands to `syscall`
  - move `isr_80h` subroutines in `idt.asm` into `syscall.asm`
- TODO: makefile for stdlib and user programs is messy


- TODO: gather system info and print on startup (memory, timer, etc)
- TODO: refactor includes to be based on project dir instead of relative...
- TODO: dockerfile? Github action?

## Limitations

- Lazy A20 line enable; Could potentially fail on some systems
- Memory fragmentation
- Only one drive supported (master)
- Missing FAT16 write
- ELF file loading loads entire file into heap memory...
- Only supports loading static ELF files
- Simple multitasking via timer

## References

- [OSDev.org](https://wiki.osdev.org/Main_Page)
- [BrokenThorn OS Dev](http://www.brokenthorn.com/Resources/OSDevIndex.html)
- [x86 and amd64 instruction set](https://www.felixcloutier.com/x86/)
- [Ralf Brown Interrupt List](https://www.ctyme.com/rbrown.htm)
- [Operating Systems: Three Easy Pieces](https://pages.cs.wisc.edu/~remzi/OSTEP/)
- [PC Interrupts: A Programmer's Reference to BIOS, DOS, and Third-Party Calls](https://www.amazon.com/PC-Interrupts-Programmers-Reference-Third-Party/dp/0201624850)
- [Executable and Linkable Format (ELF)](https://refspecs.linuxfoundation.org/elf/elf.pdf)
