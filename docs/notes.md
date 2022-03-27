
# Notes

## Memory Map

| Address | Label       | Notes |
| ------- | ----------- | ----- |
| 0x7C00  | Boot sector |       |

## Bootloader

https://wiki.osdev.org/Boot_Sequence#Loading

### Processor Modes

- Real mode: 
  - `segment:offset` memory model, 1MB of memory, no virtual memory or memory protection
  - `segment:offset` => `exact memory address = (Segment Address * 16) + offset`
  - ex: `07C0:0000 = 0x07C0 * 16 + 0 = 0x7C00`
- Protected mode
  - memory protection via descriptor tables, 4GB RAM
  - no interrupts available; write yourself. Use any interrupt will cause triple fault
  - must use descriptor tables like GDT, LDT, IDT
  - `segment:offset` addressing used alongside linear addressing
  - 32-bit registers
- Unreal mode
  - Real mode with 4GB address space of protected mode
  - switch cpu from real to protected mode, and back again after loading new descriptor
- Virtual 8086 mode
  - Protected mode with 16-bit real mode emulated

### BIOS

- https://en.wikipedia.org/wiki/BIOS_parameter_block
- Detect CPU/BIOS features (ex: `INT 0x13` extensions)
  - https://wiki.osdev.org/How_Do_I_Determine_The_Amount_Of_RAM
  - https://wiki.osdev.org/Getting_VBE_Mode_Info
