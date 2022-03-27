# Build enki-os

BIN = myos.bin
ISO = myos.iso
ISO_DIR = iso

LINKER = linker.ld
CROSS = ${HOME}/opt/cross

AS = $(CROSS)/bin/i686-elf-as
GCC = $(CROSS)/bin/i686-elf-gcc
QEMU = qemu-system-i386

CFLAGS ?= -O2 -g
CFLAGS := $(CFLAGS) -Wall -Wextra

all: 		rebuild

rebuild:	clean build

setup:
			@echo "No automated setup for now, please refer to docs/dev-setup.md"

build:
			$(AS) boot.s -o boot.o
			$(GCC) -c kernel.c -o kernel.o -std=gnu99 -ffreestanding $(CFLAGS)
			$(GCC) -T $(LINKER) -o $(BIN) -ffreestanding $(CFLAGS) -nostdlib boot.o kernel.o -lgcc

build_iso:	build
			@mkdir -p $(ISO_DIR)/grub
			cp $(BIN) $(ISO_DIR)/$(BIN)
			cp grub.cfg $(ISO_DIR)/grub/grub.cfg
			grub-mkrescue -o $(ISO) $(ISO_DIR)

qemu:
			$(QEMU) -kernel $(BIN)

qemu_iso:
			$(QEMU) -cdrom $(ISO)

clean:
			rm -f ./*.o $(ISO) $(BIN)
			rm -rf $(ISO_DIR)
