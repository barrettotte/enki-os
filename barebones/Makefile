# Build enki-os

ISO = myos.iso
BIN = myos.bin
BOOT_DIR = boot
LINKER = linker.ld
CROSS = ${HOME}/opt/cross

AS = $(CROSS)/bin/i686-elf-as
GCC = $(CROSS)/bin/i686-elf-gcc
QEMU = qemu-system-i386

all: 		rebuild

rebuild:	clean build

setup:
			@echo "No automated setup for now, please refer to docs/dev-setup.md"

build:
			$(AS) boot.s -o boot.o
			$(GCC) -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
			$(GCC) -T $(LINKER) -o $(BIN) -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

build_iso:	build
			@mkdir -p $(BOOT_DIR)/grub
			cp $(BIN) $(BOOT_DIR)/$(BIN)
			cp grub.cfg $(BOOT_DIR)/grub/grub.cfg
			grub-mkrescue -o $(ISO) $(BOOT_DIR)

qemu:
			$(QEMU) -kernel $(BIN)

qemu_iso:
			$(QEMU) -cdrom $(ISO)

clean:
			rm -f ./*.o $(ISO) $(BIN)
			rm -rf $(BOOT_DIR)
