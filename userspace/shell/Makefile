ARCH := i686
FMT  := elf

BIN_DIR := bin
OBJ_DIR := build
SRC_DIR := src

STDLIB := ../stdlib/src
TARGET := shell

CROSS := "$(HOME)/opt/cross/bin/$(ARCH)-$(FMT)"

CC := $(CROSS)-gcc
CC_INCLUDES := $(STDLIB)
CC_FLAGS := -g -ffreestanding \
			-falign-jumps -falign-functions -falign-labels -falign-loops \
			-fstrength-reduce -fomit-frame-pointer -finline-functions -fno-builtin \
			-Werror -Wno-unused-function -Wno-unused-label -Wno-cpp -Wno-unused-parameter \
			-nostdlib -nostartfiles -nodefaultlibs \
			-Wall -O0

.PHONY:	.FORCE
.FORCE:

all:	build

build:	clean
	@echo "building $(TARGET)..."
	@mkdir -p $(OBJ_DIR)/
	@mkdir -p $(BIN_DIR)/
	$(CC) -I $(CC_INCLUDES) $(CC_FLAGS) -std=gnu99 -c $(SRC_DIR)/$(TARGET).c -o $(OBJ_DIR)/$(TARGET).o
	$(CC) -g -T linker.ld -o $(BIN_DIR)/$(TARGET).elf -ffreestanding -O0 -nostdlib -fpic -g \
		$(OBJ_DIR)/$(TARGET).o ../stdlib/bin/stdlib.elf

read_elf:	build
	readelf -a $(BIN_DIR)/$(TARGET).elf

clean:
	@rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*