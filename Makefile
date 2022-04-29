OS   := enki
ARCH := i686
FMT  := elf

BIN_DIR := bin
MNT_DIR := /mnt/$(OS)
SCRIPTS_DIR := scripts

KERNEL_DIR := kernel
LIBC_DIR := libc
USER_DIR := userspace

BOOT_BIN := $(KERNEL_DIR)/bin/boot.bin
KERNEL_BIN := $(KERNEL_DIR)/bin/kernel.bin
KERNEL_OBJ := $(KERNEL_DIR)/obj/kernel_full.o
ENKI_BIN := $(BIN_DIR)/enki.bin

KERNEL_ENTRY := 0x100000
USER_ENTRY   := 0x400000

1MB := 1048576

QEMU := qemu-system-i386
QEMU_FLAGS := -drive file=$(ENKI_BIN),format=raw

.PHONY:	.FORCE
.FORCE:

all:	build img mount unmount

build:	clean
	@mkdir -p $(BIN_DIR)
	$(MAKE) -C $(KERNEL_DIR) all
	$(MAKE) -C $(USER_DIR)/stdlib all
	$(MAKE) -C $(USER_DIR)/nothing all
	$(MAKE) -C $(USER_DIR)/shell all

clean:
	rm -rf $(BIN_DIR)/*

img:
	@echo "building image..."
	dd if=$(BOOT_BIN) status=none >> $(ENKI_BIN)
	dd if=$(KERNEL_BIN) status=none >> $(ENKI_BIN)
	dd if=/dev/zero bs=$(1MB) count=16 status=none>> $(ENKI_BIN)

mount:
	@sudo mkdir -p $(MNT_DIR)
	@sudo mount -t vfat $(ENKI_BIN) $(MNT_DIR)
	@sudo cp $(USER_DIR)/nothing/bin/nothing.elf $(MNT_DIR)
	@sudo cp $(USER_DIR)/shell/bin/shell.elf $(MNT_DIR)

unmount:
	@sudo umount $(MNT_DIR)

qemu:	all
	$(QEMU) $(QEMU_FLAGS) -monitor stdio

debug_kernel:	all
	@gdb -ex 'set confirm off' \
		-ex 'add-symbol-file $(KERNEL_OBJ) $(KERNEL_ENTRY)' \
		-ex 'break kernel_main' \
		-ex 'target remote | $(QEMU) $(QEMU_FLAGS) -S -gdb stdio'

debug_userspace:	all
	@gdb -ex 'set confirm off' \
		-ex 'add-symbol-file $(USER_DIR)/shell/bin/shell.elf $(USER_ENTRY)' \
		-ex 'break main' \
		-ex 'target remote | $(QEMU) $(QEMU_FLAGS) -S -gdb stdio'

toolchain:
	@echo "Building toolchain..."
	@chmod +x $(SCRIPTS_DIR)/toolchain.sh
	$(SCRIPTS_DIR)/toolchain.sh

line_count:	all
	git ls-files | xargs wc -l
