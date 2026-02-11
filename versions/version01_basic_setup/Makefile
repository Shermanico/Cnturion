# ============================================
# Cnturion Inventory Manager - Makefile (Version 1)
# ============================================
# 
# Version 1: Basic Setup
# This file automates the compilation process
# 
# KEY CONCEPTS:
# - Variables: Store reusable values (CC, CFLAGS, etc.)
# - Targets: Rules for building specific files
# - Dependencies: Files that must exist before building
# - $@: The target filename
# - $<: The first prerequisite filename
# - $^: All prerequisites
#
# USAGE:
#   make          - Build the project
#   make clean    - Remove build artifacts
#   make run      - Build and run the application
# ============================================

# ============================================
# COMPILER AND FLAGS
# ============================================
CC = gcc

# CFLAGS: Compiler flags
# -Wall: Enable all warnings (helps catch bugs)
# -Wextra: Enable extra warnings
# -Werror: Treat warnings as errors (forces clean code)
# -std=c99: Use C99 standard (modern C)
# -Iinclude: Add include directory to header search path
# -g: Add debug information (for gdb debugger)
# -O2: Optimization level 2 (faster code)
CFLAGS = -Wall -Wextra -Werror -std=c99 -Iinclude -g -O2

# LDFLAGS: Linker flags
# -lsqlite3: Link against SQLite library (system library)
LDFLAGS = -lsqlite3

# ============================================
# DIRECTORIES
# ============================================
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DATA_DIR = data
LOG_DIR = logs

# ============================================
# SOURCE FILES
# ============================================
SRCS = $(SRC_DIR)/main.c \
        $(SRC_DIR)/database.c \
        $(SRC_DIR)/utils.c

# ============================================
# OBJECT FILES
# ============================================
OBJS = $(OBJ_DIR)/main.o \
        $(OBJ_DIR)/database.o \
        $(OBJ_DIR)/utils.o

# ============================================
# TARGET EXECUTABLE
# ============================================
TARGET = $(BIN_DIR)/cnturion

# ============================================
# BUILD RULES
# ============================================

# Default target
all: $(TARGET)

# Link executable
$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo "Linking executable..."
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories
$(BIN_DIR) $(OBJ_DIR) $(DATA_DIR) $(LOG_DIR):
	@echo "Creating directories..."
	mkdir -p $@

# ============================================
# CLEAN RULE
# ============================================

clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Clean complete"

# ============================================
# RUN RULE
# ============================================

run: $(TARGET)
	@echo "Running application..."
	./$(TARGET)

# ============================================
# PHONY TARGETS
# ============================================
.PHONY: all clean run
