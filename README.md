# enki-os

A simple/toy x86 operating system built while learning operating system concepts.

TODO: add picture of Enki with link to wiki

## Summary

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
- **Bootloader stage 1**
  - limited to 512 bytes
  - load bootloader stage 2 from boot drive (FAT12 floppy) to memory
  - transfer control to stage 2 via BIOS interrupts
- **Bootloader stage 2**; at `0x500`
  - TODO: enable address line 20 (A20)
  - TODO: install GDT and IDT
  - TODO: load kernel in memory
  - TODO: switch to protected mode
  - TODO: transfer control to kernel

## Features

TODO:

Held off at `crti.s`

## Development

See [docs/dev-setup.md](docs/dev-setup.md)

## References

- Bootloader
  - [BIOS Interrupt Calls](https://en.wikipedia.org/wiki/BIOS_interrupt_call)
  - [OSDev: Rolling Your Own Bootloader](https://wiki.osdev.org/Rolling_Your_Own_Bootloader)
  - [Carnegie Mellon CS: Writing a Bootloader from Scratch](https://www.cs.cmu.edu/~410-s07/p4/p4-boot.pdf)
  - [BIOS Parameter Block](https://wiki.osdev.org/FAT#BPB_.28BIOS_Parameter_Block.29)
  - [Interrupt Vector Table](https://wiki.osdev.org/Interrupt_Vector_Table)
  - [Cylinder-head-sector](https://en.wikipedia.org/wiki/Cylinder-head-sector)
  - [Logical block addressing](https://en.wikipedia.org/wiki/Logical_block_addressing)
  - [A20 Line](https://wiki.osdev.org/A20_Line)
- [OSDev.org](https://wiki.osdev.org/Main_Page)
- [BrokenThorn OS Dev](http://www.brokenthorn.com/Resources/OSDevIndex.html)
