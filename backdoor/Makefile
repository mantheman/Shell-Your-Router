BUILD_DIR = bin
INCLUDE_DIR = include
SOURCE_DIR = src
OBJ_DIR = obj
LIB_DIR = lib

TARGET = bd

CC = gcc
RM = rm -rf

CFLAGS = -I$(INCLUDE_DIR) -Wall -Wextra -Werror -Wpedantic -std=c99
LIBS =

HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)
SOURCES = $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS = $(patsubst $(SOURCE_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

.PHONY: debug
debug: CC = gcc
debug: CFLAGS += -g -DDEBUG
debug: executable

.PHONY: release
release: CC = mipsel-unknown-linux-uclibc-gcc # A toolchain I created with crosstool-NG.
release: CFLAGS += -s # -s (strip) flag to make the executable smaller.
release: executable

.PHONY: executable 
executable: $(BUILD_DIR)/$(TARGET)

# $@ - The file name of the target of the rule.
# $< - The name of the first prerequisite.
# $^ - The names of all the prerequisites.
$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

.PHONY: clean
clean:
	$(RM) $(BUILD_DIR)/*
	$(RM) $(OBJ_DIR)/*

# Show variables (for debug use only.)
.PHONY: show
show:
	@echo "Sources:" $(SOURCES)
	@echo "Headers:" $(HEADERS)
	@echo "Objects:" $(OBJECTS)

