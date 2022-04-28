# Build Enki OS

OS   := enki
ARCH := i686
FMT  := elf

BIN_DIR := bin
OBJ_DIR := build
SRC_DIR := src

TARGET  := $(BIN_DIR)/kernel.bin
HEADERS := $(shell find $(SRC_DIR)/* -type f -iname "*.h")

SRC_EXCL    := -not \( -path "$(SRC_DIR)/boot/*" -o -path "$(SRC_DIR)/kernel.asm" \)
SRC_TYPES   := -type f \( -iname "*.asm" -o -iname "*.c" -o -iname "*.cpp" \)
SOURCES     := $(shell find $(SRC_DIR)/* $(SRC_EXCL) $(SRC_TYPES))
OBJECTS     := $(foreach OBJECT, $(patsubst %.asm, %.asm.o, $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SOURCES)))), $(OBJ_DIR)/$(OBJECT))
KERNEL_FULL := $(OBJ_DIR)/kernel_full.o

CROSS := "$(HOME)/opt/cross/bin/$(ARCH)-$(FMT)"

AS := nasm
AS_FLAGS := -f elf -g

LD := $(CROSS)-ld
LD_FLAGS := -g -relocatable --warn-unresolved-symbols

CC := $(CROSS)-gcc
CC_FLAGS := -g -ffreestanding \
			-falign-jumps -falign-functions -falign-labels -falign-loops \
			-fstrength-reduce -fomit-frame-pointer -finline-functions -fno-builtin \
			-Werror -Wno-unused-function -Wno-unused-label -Wno-cpp -Wno-unused-parameter \
			-nostdlib -nostartfiles -nodefaultlibs \
			-Wall -O0
CC_INCLUDES := -I $(SRC_DIR)

CPP := $(CROSS)-g++
CPP_FLAGS := -g -ffreestanding \
			 -fno-builtin \
			 -nostdlib -nostartfiles -nodefaultlibs \
			 -Wall

QEMU := qemu-system-i386
QEMU_FLAGS := -drive file=$(BIN_DIR)/os.bin,format=raw

$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(AS) -f bin $(SRC_DIR)/boot/boot.asm -o $(BIN_DIR)/boot.bin
	$(AS) $(AS_FLAGS) $(SRC_DIR)/kernel.asm -o $(OBJ_DIR)/kernel.asm.o
	@echo "linking...objects: $+"
	$(LD) $(LD_FLAGS) $(OBJ_DIR)/kernel.asm.o $+ -o $(KERNEL_FULL)
	$(CC) $(CC_FLAGS) -T $(SRC_DIR)/linker.ld -o $@ -ffreestanding -O0 -nostdlib $(KERNEL_FULL)

$(OBJ_DIR)/%.asm.o: %.asm
	@mkdir -p $(@D)
	$(AS) $(AS_FLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CC_INCLUDES) $(CC_FLAGS) -std=gnu99 -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo "No C++ support...unable to compile $<"

.PHONY:	.FORCE
.FORCE:

all:	build

build:	clean $(TARGET) img userspace mount unmount

clean:
	rm -rf $(BIN_DIR)/* $(OBJ_DIR)/*

img:
	@echo "building image..."
	dd if=$(BIN_DIR)/boot.bin status=none >> $(BIN_DIR)/os.bin
	dd if=$(BIN_DIR)/kernel.bin status=none >> $(BIN_DIR)/os.bin
	dd if=/dev/zero bs=1048576 count=16 status=none>> $(BIN_DIR)/os.bin

userspace:	.FORCE
	$(MAKE) -C userspace/stdlib all
	$(MAKE) -C userspace/nothing all
	$(MAKE) -C userspace/shell all

qemu:	build
	$(QEMU) $(QEMU_FLAGS) -monitor stdio

mount:
	@sudo mkdir -p /mnt/$(OS)
	@sudo mount -t vfat $(BIN_DIR)/os.bin /mnt/$(OS)
	@sudo cp hello.txt /mnt/$(OS)
	@sudo cp userspace/nothing/bin/nothing.elf /mnt/$(OS)
	@sudo cp userspace/shell/bin/shell.elf /mnt/$(OS)

unmount:
	@sudo umount /mnt/$(OS)

debug_kernel:	build
	@gdb -ex 'set confirm off' \
		-ex 'add-symbol-file $(KERNEL_FULL) 0x100000' \
		-ex 'break kernel_main' \
		-ex 'target remote | $(QEMU) $(QEMU_FLAGS) -S -gdb stdio'

debug_userspace:	build
	@gdb -ex 'set confirm off' \
		-ex 'add-symbol-file userspace/shell/bin/shell.elf 0x400000' \
		-ex 'break main' \
		-ex 'target remote | $(QEMU) $(QEMU_FLAGS) -S -gdb stdio'

toolchain:
	@echo "Building toolchain..."
	@chmod +x scripts/toolchain.sh
	scripts/toolchain.sh

line_count:
	git ls-files | xargs wc -l
