# enki-os

A toy x86 operating system.

This is my first dive into the deep world of Operating Systems.

TODO: add picture of Enki with link to wiki

## Features / Goals

- [x] Custom bootloader
- [x] FAT-16 file system
- [x] Keyboard driver
- [ ] Partial LibC
- [ ] Userland with basic shell
- [ ] Multitasking
- [ ] Userland ELF loading

## Building Locally

- Install dependencies and build cross compiler: `make toolchain`
- Build OS image: `make`
- Build OS image and launch in QEMU: `make qemu`
- Build OS image and launch with GDB connected to QEMU: `make debug`

## Development

Debug via GDB and QEMU with `make debug`

```sh
# burn to USB
sudo fdisk -l                     # find USB disk
sudo dd if=boot.bin of=/dev/sdb   # overwrite USB's first sector with our bootloader
```

## TODO Items

- TODO: can I make a docker image for OS development ? vagrant?
- TODO: ASCII art on launch
- TODO: three shell commands
- TODO: one simple game
- TODO: multiboot header?
- TODO: move `tty_*` functions out of `kernel.c`
- TODO: use `#pragma once` instead of `ifndef` guard?
- TODO: comments on all C functions
- TODO: comment asm
- TODO: double check `static` used where needed
- TODO: remove refs to `<stddef.h>` and any other `<std*.h>` file -> make a `include/types.h`
- TODO: implement a reasonable amount of stdlib - https://en.cppreference.com/w/c/header
- TODO: reevaluate use of `goto`
- TODO: compare with osdev meatyskeleton and the likes
- TODO: refactor some paths to `stdlib` (ex: string, memory, )
  - `io` to `cpu/`
  - `string` to `stdlib/`
  - `kernel.*` to `kernel/`
- TODO: userspace program makefile hardcoded arch/fmt...
- TODO: rename `isr_80h` to `syscall`?
  - move `idt.c` `isr_80h` commands to `syscall`
  - move `isr_80h` subroutines in `idt.asm` into `syscall.asm`
- TODO: for stdlib headers consider:
  - `#ifdef __cplusplus` and `extern "C" {`
- TODO: review https://wiki.osdev.org/Meaty_Skeleton
- TODO: gather system info and print on startup

## References

- [OSDev.org](https://wiki.osdev.org/Main_Page)
- [BrokenThorn OS Dev](http://www.brokenthorn.com/Resources/OSDevIndex.html)
- [x86 and amd64 instruction set](https://www.felixcloutier.com/x86/)
- [Ralf Brown Interrupt List](https://www.ctyme.com/rbrown.htm)
- [Operating Systems: Three Easy Pieces](https://pages.cs.wisc.edu/~remzi/OSTEP/)
- [PC Interrupts: A Programmer's Reference to BIOS, DOS, and Third-Party Calls](https://www.amazon.com/PC-Interrupts-Programmers-Reference-Third-Party/dp/0201624850)

