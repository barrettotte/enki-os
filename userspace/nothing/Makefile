CC := $(HOME)/opt/cross/bin/i686-elf-gcc
AS := nasm

.PHONY:	.FORCE
.FORCE:

all:	build

build:	clean
	@echo "building nothing..."
	@mkdir -p build/
	@mkdir -p bin/
	$(AS) -f elf nothing.asm -o build/nothing.o
	$(CC) -g -T linker.ld -o bin/nothing.bin -ffreestanding -O0 -nostdlib -fpic -g build/nothing.o

clean:
	@rm -rf build/* bin/*