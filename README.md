# enki-os

A basic x86 kernel to learn how operating systems get from BIOS to userland.

This is my first dive into the deep world of operating systems. 
I hope to make another kernel in the future using the things I learned here. 
This one still needs a lot of care it probably won't get.

Named after [Enki](https://en.wikipedia.org/wiki/Enki), 
Sumerian god of water, knowledge, crafts, and creation. 

## Features / Goals

- [x] Custom bootloader
- [x] FAT-16 file system (except `fwrite`)
- [x] PS/2 keyboard driver
- [x] Static ELF file loading
- [x] Partial LibC
- [x] Process create/manage via TSS
- [x] Userland with basic shell
- [x] Basic Multitasking (timer based)

## Building Locally

- Install dependencies and build cross compiler: `make toolchain`
- Build OS image: `make`
- Build OS image and launch in QEMU: `make qemu`
- Build OS image and launch with GDB connected to QEMU: `make debug`

## TODO Items

- Shift key
- Scrolling terminal
- Clear terminal
- Init Floating Point Unit (FPU)
- Reboot command
- Gather system info and print on startup (memory, timer, etc)
- Stack smash protector
- Multiboot
- Dockerfile with toolchain installed? Github action?

## Limitations

- Lazy A20 line enable; Could potentially fail on some systems
- Memory fragmentation
- Only one drive supported (master - drive 0)
- Missing FAT16 write
- ELF file loading loads entire file into heap memory...
- Only supports loading static ELF files
- Simple multitasking via timer
- Limited process control (no fork or exec)
- Probably a lot more I'm forgetting...

## References

- [OSDev.org](https://wiki.osdev.org/Main_Page)
- [BrokenThorn OS Dev](http://www.brokenthorn.com/Resources/OSDevIndex.html)
- [x86 and amd64 instruction set](https://www.felixcloutier.com/x86/)
- [Ralf Brown Interrupt List](https://www.ctyme.com/rbrown.htm)
- [Operating Systems: Three Easy Pieces](https://pages.cs.wisc.edu/~remzi/OSTEP/)
- [Little Book About OS Development](https://littleosbook.github.io/)
- [PC Interrupts: A Programmer's Reference to BIOS, DOS, and Third-Party Calls](https://www.amazon.com/PC-Interrupts-Programmers-Reference-Third-Party/dp/0201624850)
- [Executable and Linkable Format (ELF)](https://refspecs.linuxfoundation.org/elf/elf.pdf)
