# enki-os

A toy x86 operating system.

This is my first dive into the deep world of Operating Systems.

TODO: add picture of Enki with link to wiki

## Features

TODO:

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
- TODO: use `#pragma once` instead of `ifndef` guard?
- TODO: consider use embedded asm in C instead of the separate asm files?
- TODO: comments on all C functions
- TODO: comment asm
- TODO: double check `static` used where needed
- TODO: remove refs to `<stddef.h>` and any other `<std*.h>` file
- TODO: implement a reasonable amount of stdlib - https://en.cppreference.com/w/c/header
- TODO: reevaluate use of `goto`
- TODO: compare with osdev meatyskeleton and the likes
- TODO: refactor some paths to `stdlib` (ex: string, memory, )
  - `io` to `cpu/`
  - `string` to `stdlib/`
  - `kernel.*` to `kernel/`
- TODO: userspace program makefile hardcoded arch/fmt

## References

- [OSDev.org](https://wiki.osdev.org/Main_Page)
- [BrokenThorn OS Dev](http://www.brokenthorn.com/Resources/OSDevIndex.html)
- [x86 and amd64 instruction set](https://www.felixcloutier.com/x86/)
- [BIOS Interrupt Calls](https://en.wikipedia.org/wiki/BIOS_interrupt_call)
- [Cylinder-head-sector](https://en.wikipedia.org/wiki/Cylinder-head-sector)
- [Logical block addressing](https://en.wikipedia.org/wiki/Logical_block_addressing)
