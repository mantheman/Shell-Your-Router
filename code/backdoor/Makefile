BUILD_DIR = bin
INCLUDE_DIR = include
SOURCE_DIR = src
OBJ_DIR = obj
LIB_DIR = lib

TARGET = secret_shell

FIRMWARE_ROOT_DIR = /mnt/c/Users/ykrin/Projects/embedded/firmware-mod-kit/fmk/rootfs

# CC = gcc
CC = /mnt/c/Users/ykrin/Projects/embedded/tools/buildroot-2020.02.3/output/host/bin/mipsel-linux-gcc
RM = rm -rf

CFLAGS = -I$(INCLUDE_DIR) -Wall -Wextra -Werror -std=c99
LIBS = 

HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)
SOURCES = $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS = $(patsubst $(SOURCE_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

.PHONY: default all
default: $(BUILD_DIR)/$(TARGET)
all: default

# $@ - The file name of the target of the rule.
# $< - The name of the first prerequisite.
# $^ - The names of all the prerequisites.
$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

.PHONY: clean
clean:
	$(RM) $(BUILD_DIR)/*
	$(RM) $(OBJ_DIR)/*

.PHONY: install
install:
	cp $(BUILD_DIR)/$(TARGET) $(FIRMWARE_ROOT_DIR)/bin/

# Show variables (for debug use only.)
.PHONY: show
show:
	@echo "Sources:" $(SOURCES)
	@echo "Headers:" $(HEADERS)
	@echo "Objects:" $(OBJECTS)

