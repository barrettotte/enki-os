# Build Enki OS

OS   := enki
ARCH := x86_64

BIN   := bin
BUILD := build
OUT   := out
SRC   := src
BOOT  := $(SRC)/boot

AS   := nasm
QEMU := qemu-system-$(ARCH)
QEMU_FLAGS := -drive file=$(BIN)/boot.bin,format=raw \
		      -d guest_errors -D $(OUT)/error.log

.PHONY:		.FORCE
.FORCE:

all:		build

build:		clean boot
	
boot:
			$(AS) -f bin $(BOOT)/boot.asm -o $(BIN)/boot.bin

qemu:		build
			$(QEMU) $(QEMU_FLAGS)

debug:		build
			gdb -ex 'target remote | $(QEMU) $(QEMU_FLAGS) -S -gdb stdio'

clean:
			rm -rf $(BIN)/*.bin
			rm -rf $(OUT)/*

toolchain:
			chmod +x scripts/toolchain.sh
			sudo scripts/toolchain.sh $(ARCH)