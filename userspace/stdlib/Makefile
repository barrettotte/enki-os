OS := enki
ARCH := i686
FMT  := elf

BIN_DIR := bin
OBJ_DIR := build
SRC_DIR := src

TARGET := stdlib

CROSS := "$(HOME)/opt/cross/bin/$(ARCH)-$(FMT)"

AS := nasm
AS_FLAGS := -f elf

LD := $(CROSS)-ld
LD_FLAGS := -m elf_i386 -relocatable

CC := $(CROSS)-gcc
CC_INCLUDES := $(SRC_DIR)
CC_FLAGS := -g \
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
	$(AS) $(AS_FLAGS) $(SRC_DIR)/start.asm -o $(OBJ_DIR)/start.asm.o
	$(AS) $(AS_FLAGS) $(SRC_DIR)/$(OS).asm -o $(OBJ_DIR)/$(OS).asm.o
	$(CC) -I $(CC_INCLUDES) $(CC_FLAGS) -std=gnu99 -c $(SRC_DIR)/$(OS).c -o $(OBJ_DIR)/$(OS).o
	$(CC) -I $(CC_INCLUDES) $(CC_FLAGS) -std=gnu99 -c $(SRC_DIR)/$(TARGET).c -o $(OBJ_DIR)/$(TARGET).o
	$(CC) -I $(CC_INCLUDES) $(CC_FLAGS) -std=gnu99 -c $(SRC_DIR)/start.c -o $(OBJ_DIR)/start.o
	$(CC) -I $(CC_INCLUDES) $(CC_FLAGS) -std=gnu99 -c $(SRC_DIR)/stdlib.c -o $(OBJ_DIR)/stdlib.o
	$(CC) -I $(CC_INCLUDES) $(CC_FLAGS) -std=gnu99 -c $(SRC_DIR)/stdio.c -o $(OBJ_DIR)/stdio.o
	$(CC) -I $(CC_INCLUDES) $(CC_FLAGS) -std=gnu99 -c $(SRC_DIR)/memory.c -o $(OBJ_DIR)/memory.o
	$(CC) -I $(CC_INCLUDES) $(CC_FLAGS) -std=gnu99 -c $(SRC_DIR)/string.c -o $(OBJ_DIR)/string.o
	$(LD) $(LD_FLAGS) $(OBJ_DIR)/start.asm.o $(OBJ_DIR)/$(OS).asm.o $(OBJ_DIR)/$(OS).o \
		$(OBJ_DIR)/stdlib.o $(OBJ_DIR)/start.o $(OBJ_DIR)/stdio.o $(OBJ_DIR)/memory.o $(OBJ_DIR)/string.o \
		-o $(BIN_DIR)/$(TARGET).elf

clean:
	@rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*