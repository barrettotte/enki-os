ARCH := i686
FMT  := elf

BIN_DIR := bin
OBJ_DIR := build
SRC_DIR := .

TARGET := $(BIN_DIR)/$(notdir $(CURDIR))

CC := $(HOME)/opt/cross/bin/$(ARCH)-$(FMT)-gcc

.PHONY:	.FORCE
.FORCE:

all:	build

clean:
	@rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*

build:	clean
	@echo "building $(TARGET)..."
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)
	nasm -f elf test.asm -o $(OBJ_DIR)/test.o
	$(CC) -g -T linker.ld -o $(TARGET) \
		-ffreestanding -O0 -nostdlib -fpic -g $(OBJ_DIR)/test.o
