# Build Enki OS

OS    := enki

ARCH  := i686
TARGET := $(ARCH)-elf

BIN   := bin
BUILD := build
OUT   := out
SRC   := src

AS     := nasm
LD     := $(TARGET)-ld
GCC    := $(TARGET)-gcc

QEMU := qemu-system-x86_64
QEMU_FLAGS := -drive file=$(BIN)/os.bin,format=raw

GCC_INCLUDES := -I src/
GCC_FLAGS := -g -ffreestanding \
			 -falign-jumps -falign-functions -falign-labels -falign-loops \
			 -fstrength-reduce -fomit-frame-pointer -finline-functions -fno-builtin \
			 -Werror -Wno-unused-function -Wno-unused-label -Wno-cpp -Wno-unused-parameter \
			 -nostdlib -nostartfiles -nodefaultlibs \
			 -Wall -O0 -Iinc

OBJECTS := $(BUILD)/kernel.asm.o $(BUILD)/kernel.o

.PHONY:			.FORCE
.FORCE:

all:			build

build:          clean build_kernel link os_img

build_boot:
				$(AS) -f bin $(SRC)/boot/boot.asm -o $(BIN)/boot.bin

build_kernel:	build_boot
				$(AS) -f elf -g src/kernel.asm -o $(BUILD)/kernel.asm.o
				$(GCC) $(GCC_INCLUDES) $(GCC_FLAGS) -std=gnu99 -c $(SRC)/kernel.c -o $(BUILD)/kernel.o

link:			
				$(LD) -g -relocatable $(OBJECTS) -o $(BUILD)/kernel_full.o
				$(GCC) $(GCC_FLAGS) -T $(SRC)/linker.ld -o $(BIN)/kernel.bin $(BUILD)/kernel_full.o

os_img:
				dd if=$(BIN)/boot.bin >> $(BIN)/os.bin
				dd if=$(BIN)/kernel.bin >> $(BIN)/os.bin
				dd if=/dev/zero bs=512 count=100 >> $(BIN)/os.bin

qemu:			build_img
				$(QEMU) $(QEMU_FLAGS)

debug:			build_img
				gdb -ex 'set confirm off' \
				    -ex 'add-symbol-file $(BUILD)/kernel_full.o 0x100000' \
					-ex 'break kernel_main' \
				    -ex 'target remote | $(QEMU) $(QEMU_FLAGS) -S -gdb stdio'

clean:
				rm -rf $(BIN)/* $(BUILD)/* $(OUT)/*

toolchain:
				chmod +x scripts/toolchain.sh
				scripts/toolchain.sh
