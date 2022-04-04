# enki-os

A simple/toy x86 operating system built while learning operating system concepts.

TODO: add picture of Enki with link to wiki

## Features

TODO:

## Building Locally

- Install dependencies and build cross compiler: `make toolchain`
- Build OS image: `make`
- Build OS image and launch in QEMU `make qemu`

## Development

Debug via GDB and QEMU with `make debug`

```sh
# burn to USB
sudo fdisk -l                     # find USB disk
sudo dd if=boot.bin of=/dev/sdb   # overwrite USB's first sector with our bootloader
```

## TODO Items

- TODO: can I make a docker image for OS development ?
- TODO: use `#pragma once` instead of `ifndef` guard?

## OS Startup

- User presses power button
  - motherboard routes "on" signal to power supply
  - power supply supplies power to the rest of the system
  - power supply routes "power good" signal to motherboard
- CPU executes **BIOS** (Basic Input/Output System) directly from ROM
  - BIOS loads itself into RAM, then continues execution from RAM
  - initiates POST (Power On Self Test); runs power, device, and memory tests
  - creates interrupt vector table (IVT) and does more hardware tests
  - tries to find bootloader; searches all storage mediums for boot signature `0x55AA`
  - loads bootloader into RAM at `0x7C00` and absolute jumps to this location
- **Bootloader**
  - limited to 512 bytes
  - TODO:

## References

- Bootloader
  - [BIOS Interrupt Calls](https://en.wikipedia.org/wiki/BIOS_interrupt_call)
  - [OSDev: Rolling Your Own Bootloader](https://wiki.osdev.org/Rolling_Your_Own_Bootloader)
  - [BIOS Parameter Block](https://wiki.osdev.org/FAT#BPB_.28BIOS_Parameter_Block.29)
  - [Interrupt Vector Table](https://wiki.osdev.org/Interrupt_Vector_Table)
  - [Cylinder-head-sector](https://en.wikipedia.org/wiki/Cylinder-head-sector)
  - [Logical block addressing](https://en.wikipedia.org/wiki/Logical_block_addressing)
  - [Global Descriptor Table](https://wiki.osdev.org/Global_Descriptor_Table)
  - [A20 Line](https://wiki.osdev.org/A20_Line)
- Kernel
  - [Interrupt Descriptor Table](https://wiki.osdev.org/Interrupt_Descriptor_Table)
- [OSDev.org](https://wiki.osdev.org/Main_Page)
- [BrokenThorn OS Dev](http://www.brokenthorn.com/Resources/OSDevIndex.html)
