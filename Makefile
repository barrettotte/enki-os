# TODO: add rules

all:
	nasm -f bin src/boot/boot.asm -o bin/boot.bin
	
qemu:
	qemu-system-x86_64 -hda bin/boot.bin

debug:
	gdb -ex 'target remote | qemu-system-x86_64 -hda bin/boot.bin -S -gdb stdio'

clean:
	rm -rf bin/*.bin
