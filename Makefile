OS   := enki
ARCH := i686
FMT  := elf

KERNEL_DIR  := kernel
LIBC_DIR    := libc
PGM_DIR     := programs
BIN_DIR     := bin
MNT_DIR     := /mnt/$(OS)
SCRIPTS_DIR := scripts

BOOT_BIN   := $(KERNEL_DIR)/bin/boot.bin
KERNEL_BIN := $(KERNEL_DIR)/bin/kernel.bin
KERNEL_OBJ := $(KERNEL_DIR)/obj/kernel_full.o
ENKI_BIN   := $(BIN_DIR)/enki.bin
PGMS       := shell repeat hello

KERNEL_ENTRY := 0x100000
PGM_ENTRY    := 0x400000
1MB          := 1048576

QEMU       := qemu-system-i386
QEMU_FLAGS := -drive file=$(ENKI_BIN),format=raw

.PHONY:	.FORCE
.FORCE:

all:	build img mount unmount

build:	clean
	$(MAKE) -C $(LIBC_DIR) all
	$(MAKE) -C $(KERNEL_DIR) all
	$(foreach pgm, $(PGMS), $(MAKE) -C $(PGM_DIR)/$(pgm) all ; )

clean:
	@rm -rf $(BIN_DIR)/*

img:
	@echo "building image..."
	@mkdir -p $(BIN_DIR)
	dd if=$(BOOT_BIN) status=none >> $(ENKI_BIN)
	dd if=$(KERNEL_BIN) status=none >> $(ENKI_BIN)
	dd if=/dev/zero bs=$(1MB) count=16 status=none>> $(ENKI_BIN)

mount:
	@sudo mkdir -p $(MNT_DIR)
	@sudo mount -t vfat $(ENKI_BIN) $(MNT_DIR)
	@$(foreach pgm, $(PGMS), sudo cp $(PGM_DIR)/$(pgm)/bin/$(pgm) $(MNT_DIR) ; )

unmount:
	@sudo umount $(MNT_DIR)

qemu:	all
	$(QEMU) $(QEMU_FLAGS) -monitor stdio

debug_kernel:	all
	@gdb -ex 'set confirm off' \
		-ex 'add-symbol-file $(KERNEL_OBJ) $(KERNEL_ENTRY)' \
		-ex 'break kernel_main' \
		-ex 'target remote | $(QEMU) $(QEMU_FLAGS) -S -gdb stdio'

debug_shell:	all
	@gdb -ex 'set confirm off' \
		-ex 'add-symbol-file $(PGM_DIR)/shell/bin/shell $(PGM_ENTRY)' \
		-ex 'break main' \
		-ex 'target remote | $(QEMU) $(QEMU_FLAGS) -S -gdb stdio'

toolchain:
	@echo "Building toolchain..."
	@chmod +x $(SCRIPTS_DIR)/toolchain.sh
	$(SCRIPTS_DIR)/toolchain.sh

line_count:
	git ls-files | xargs wc -l
