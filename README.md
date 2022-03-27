# enki-os

A simple/toy x86 operating system built while learning operating system concepts.

TODO: add picture of Enki with link to wiki

## Features

TODO:

Held off at `crti.s`

## Development

See [docs/dev-setup.md](docs/dev-setup.md)

- Boot directly - `make build && make qemu`
- Boot from ISO - `make build_iso && make qemu_iso`

## References

- Bootloader
  - [BIOS Interrupt Calls](https://en.wikipedia.org/wiki/BIOS_interrupt_call)
  - [OSDev: Rolling Your Own Bootloader](https://wiki.osdev.org/Rolling_Your_Own_Bootloader)
  - [Carnegie Mellon CS: Writing a Bootloader from Scratch](https://www.cs.cmu.edu/~410-s07/p4/p4-boot.pdf)
- [OSDev.org](https://wiki.osdev.org/Main_Page)
- [BrokenThorn OS Dev](http://www.brokenthorn.com/Resources/OSDevIndex.html)
