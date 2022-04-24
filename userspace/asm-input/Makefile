TARGET := asm-input

CC := $(HOME)/opt/cross/bin/i686-elf-gcc
AS := nasm

.PHONY:	.FORCE
.FORCE:

all:	build

build:	clean
	@echo "building $(TARGET)..."
	@mkdir -p build/
	@mkdir -p bin/
	$(AS) -f elf $(TARGET).asm -o build/$(TARGET).o
	$(CC) -g -T linker.ld -o bin/$(TARGET).elf -ffreestanding -O0 -nostdlib -fpic -g build/$(TARGET).o

read_elf:	build
	readelf -S bin/$(TARGET).elf

clean:
	@rm -rf build/* bin/*