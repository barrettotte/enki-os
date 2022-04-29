OS   := enki
ARCH := i686
FMT  := elf

BIN_DIR := bin
OBJ_DIR := build
SRC_DIR := .

TARGET  := $(BIN_DIR)/libc
HEADERS := $(shell find $(SRC_DIR)/* -type f -iname "*.h")

SRC_TYPES   := -type f \( -iname "*.asm" -o -iname "*.c" -o -iname "*.cpp" \)
SOURCES     := $(shell find $(SRC_DIR)/* $(SRC_TYPES))
OBJECTS     := $(foreach OBJECT, $(patsubst %.asm, %.asm.o, $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SOURCES)))), $(OBJ_DIR)/$(OBJECT))

CROSS := "$(HOME)/opt/cross/bin/$(ARCH)-$(FMT)"

AS := nasm
AS_FLAGS := -f elf -g

LD := $(CROSS)-ld
LD_FLAGS := -g -relocatable --warn-unresolved-symbols

CC := $(CROSS)-gcc
CC_FLAGS := -g \
			-falign-jumps -falign-functions -falign-labels -falign-loops \
			-fstrength-reduce -fomit-frame-pointer -finline-functions -fno-builtin \
			-Werror -Wno-unused-function -Wno-unused-label -Wno-cpp -Wno-unused-parameter \
			-nostdlib -nostartfiles -nodefaultlibs \
			-Wall -O0
CC_INCLUDES := -I $(SRC_DIR)/include

CPP := $(CROSS)-g++
CPP_FLAGS := -g -fno-builtin -nostdlib -nostartfiles -nodefaultlibs -Wall

$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(LD) $(LD_FLAGS) $(OBJECTS) -o $@

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

all:	clean build

build:	$(TARGET)

clean:
	rm -rf $(BIN_DIR)/* $(OBJ_DIR)/*
