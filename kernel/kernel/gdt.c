#include <kernel/gdt.h>
#include <kernel/panic.h>

// convert structured GDT entry to unstructured entry
void encode_gdt_entry(uint8_t* target, struct gdt_structured src) {
    if (src.limit > 65536 && ((src.limit & 0xFFF) != 0xFFF)) {
        panic("encode_gdt_entry: Invalid argument\n");
    }
    target[6] = 0x40;
    if (src.limit > 65536) {
        src.limit = src.limit >> 12;
        target[6] = 0xC0;
    }

    target[0] = src.limit & 0xFF;
    target[1] = (src.limit >> 8) & 0xFF;
    target[6] |= (src.limit >> 16) & 0x0F;

    target[2] = src.base & 0xFF;
    target[3] = (src.base >> 8) & 0xFF;
    target[4] = (src.base >> 16) & 0xFF;
    target[7] = (src.base >> 24) & 0xFF;

    target[5] = src.type;
}

void gdt_unstructure(struct gdt* gdt, struct gdt_structured* gdt_s, int total_entries) {
    for (int i = 0; i < total_entries; i++) {
        encode_gdt_entry((uint8_t *)&gdt[i], gdt_s[i]);
    }
}
