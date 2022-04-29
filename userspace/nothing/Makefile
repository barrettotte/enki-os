ARCH := i686
FMT  := elf

BIN_DIR := bin
OBJ_DIR := build
SRC_DIR := .

LIBC_DIR := ../../libc
LIBC_BIN := $(LIBC_DIR)/bin/libc
TARGET := $(BIN_DIR)/nothing

SRC_TYPES   := -type f \( -iname "*.asm" -o -iname "*.c" -o -iname "*.cpp" \)
SOURCES     := $(shell find $(SRC_DIR)/* $(SRC_TYPES))
OBJECTS     := $(foreach OBJECT, $(patsubst %.asm, %.asm.o, $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SOURCES)))), $(OBJ_DIR)/$(OBJECT))

CROSS := "$(HOME)/opt/cross/bin/$(ARCH)-$(FMT)"

CC := $(CROSS)-gcc
CC_INCLUDES := -I $(SRC_DIR)/libc
CC_FLAGS := -g -ffreestanding \
			-falign-jumps -falign-functions -falign-labels -falign-loops \
			-fstrength-reduce -fomit-frame-pointer -finline-functions -fno-builtin \
			-Werror -Wno-unused-function -Wno-unused-label -Wno-cpp -Wno-unused-parameter \
			-nostdlib -nostartfiles -nodefaultlibs \
			-Wall -O0

.PHONY:	.FORCE
.FORCE:

all:	build

build:	clean install_headers $(TARGET)

clean:
	@rm -rf $(OBJ_DIR)/* $(BIN_DIR)/* $(SRC_DIR)/libc

install_headers:
	@mkdir -p $(SRC_DIR)/libc
	cp -R --preserve=timestamps $(LIBC_DIR)/include/* $(SRC_DIR)/libc

$(TARGET): $(OBJECTS)
	@echo "building $(TARGET)..."
	@mkdir -p $(@D)
	$(CC) -g -T $(SRC_DIR)/linker.ld -o $(TARGET) -ffreestanding -O0 -nostdlib -fpic -g \
		$(OBJECTS) $(LIBC_BIN)

$(OBJ_DIR)/%.asm.o: %.asm
	@mkdir -p $(@D)
	$(AS) $(AS_FLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CC_INCLUDES) $(CC_FLAGS) -std=gnu99 -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo "No C++ support...unable to compile $<"

read_elf:	build
	readelf -a $(BIN_DIR)/$(TARGET)
