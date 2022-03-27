# Build bootloader binaries

AS = nasm
QEMU = qemu-system-i386

.PHONY:		.FORCE
.FORCE:

all: 		rebuild

rebuild:	clean build

build:
			$(AS) -f bin boot.s -o boot1.bin

qemu:
			$(QEMU) -drive format=raw,index=0,if=floppy,file=boot1.bin

clean:
			-rm -f *.bin
