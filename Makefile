# Default build and output directories
BUILD_DIR ?= build
OUTPUT_DIR ?= bin
SOURCE_DIR ?= src

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -ljson-c -lyaml

# Source and object files
SRC = main.c
OBJ = $(BUILD_DIR)/main.o
BIN = $(OUTPUT_DIR)/yedit

# Rules
all: $(BUILD_DIR) $(OUTPUT_DIR) $(BIN)

# Create build and output directories if not exist
$(BUILD_DIR) $(OUTPUT_DIR):
	mkdir -p $@

# Compile the object file
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link the binary
$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Clean up build and output directories
clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_DIR)

install:
	cp $(BIN) /usr/local/bin

.PHONY: all clean
