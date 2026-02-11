# ============================================
# Cnturion Inventory Manager - Makefile
# ============================================
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
#   make test     - Build and run tests
#   make init-db  - Initialize the database
# ============================================

# ============================================
# COMPILER AND FLAGS
# ============================================
# CC: The C compiler to use (gcc is standard)
CC = gcc

# CFLAGS: Compiler flags
# -Wall: Enable all warnings (helps catch bugs)
# -Wextra: Enable extra warnings
# -Wno-implicit-function-declaration: Don't treat implicit declarations as errors (standard library functions)
# -Wno-int-to-pointer-cast: Don't warn about int to pointer casts
# -Wno-format-truncation: Don't warn about format truncation (snprintf is safe)
# -Werror: Treat other warnings as errors (forces clean code)
# -std=c99: Use C99 standard (modern C)
# -Iinclude: Add include directory to header search path
# -g: Add debug information (for gdb debugger)
# -O2: Optimization level 2 (faster code)
CFLAGS = -Wall -Wextra -Wno-implicit-function-declaration -Wno-int-to-pointer-cast -Wno-format-truncation -Werror -std=c99 -Iinclude -g -O2 -Wno-aggressive-loop-optimizations

# LDFLAGS: Linker flags
# -lsqlite3: Link against SQLite library (system library)
# -lpthread: Link against pthread (needed for some SQLite features)
# -lcrypt: Link against crypt library for password hashing
# -lcrypto: Link against OpenSSL crypto library for SHA256
LDFLAGS = -lsqlite3 -lpthread -lcrypt -lcrypto

# ============================================
# DIRECTORIES
# ============================================
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include
LIB_DIR = lib
DATA_DIR = data
LOGS_DIR = logs

# ============================================
# SOURCE FILES (.c files)
# ============================================
# List all source files that need to be compiled
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/ui.c \
          $(SRC_DIR)/auth.c \
          $(SRC_DIR)/inventory.c \
          $(SRC_DIR)/users.c \
          $(SRC_DIR)/database.c \
          $(SRC_DIR)/security.c \
          $(SRC_DIR)/validator.c \
          $(SRC_DIR)/logger.c \
          $(SRC_DIR)/utils.c

# ============================================
# OBJECT FILES (.o files)
# ============================================
# Object files are compiled source files
# Pattern rule: $(SRC_DIR)/%.c → $(OBJ_DIR)/%.o
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# ============================================
# TARGETS
# ============================================

# Default target (runs when you type 'make')
all: directories $(BIN_DIR)/cnturion

# Create necessary directories if they don't exist
directories:
	@echo "Creating directories..."
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(DATA_DIR)
	@mkdir -p $(LOGS_DIR)
	@mkdir -p $(LIB_DIR)
	@echo "Directories created."

# Main executable target
# Depends on all object files
$(BIN_DIR)/cnturion: $(OBJECTS)
	@echo "Linking executable..."
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# Pattern rule for compiling .c files to .o files
# $< is the source file, $@ is the object file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================
# UTILITY TARGETS
# ============================================

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Clean complete."

# Clean everything including database and logs
clean-all: clean
	@echo "Cleaning all data..."
	rm -f $(DATA_DIR)/*.db $(DATA_DIR)/*.db-shm $(DATA_DIR)/*.db-wal
	rm -f $(LOGS_DIR)/*.log
	@echo "All clean."

# Run the application
run: all
	@echo "Running Cnturion Inventory Manager..."
	$(BIN_DIR)/cnturion

# ============================================
# TEST TARGETS
# ============================================

# Test source files
TEST_SOURCES = $(SRC_DIR)/test_auth.c \
               $(SRC_DIR)/test_validator.c \
               $(SRC_DIR)/test_database.c

TEST_OBJECTS = $(TEST_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Build tests
test: directories $(TEST_OBJECTS) $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS))
	@echo "Building tests..."
	$(CC) $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_OBJECTS) -o $(BIN_DIR)/test_runner $(LDFLAGS)
	@echo "Running tests..."
	$(BIN_DIR)/test_runner

# ============================================
# DATABASE INITIALIZATION
# ============================================

# Initialize the database with schema
init-db: all
	@echo "Initializing database..."
	$(BIN_DIR)/cnturion --init-db
	@echo "Database initialized."

# ============================================
# DOCUMENTATION
# ============================================

# Generate documentation (requires Doxygen)
docs:
	@echo "Generating documentation..."
	doxygen Doxyfile 2>/dev/null || echo "Doxygen not installed or Doxyfile not found"
	@echo "Documentation generated in docs/html/"

# ============================================
# HELP TARGET
# ============================================

help:
	@echo "Cnturion Inventory Manager - Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  make          - Build the project"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make clean-all- Remove all generated files"
	@echo "  make run      - Build and run the application"
	@echo "  make test     - Build and run tests"
	@echo "  make init-db  - Initialize the database"
	@echo "  make docs     - Generate documentation"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Project structure:"
	@echo "  src/          - Source files"
	@echo "  include/      - Header files"
	@echo "  obj/          - Object files (generated)"
	@echo "  bin/          - Executables (generated)"
	@echo "  data/         - Database files"
	@echo "  logs/         - Log files"
	@echo "  lib/          - External libraries"

# ============================================
# PHONY TARGETS
# ============================================
# Phony targets are not actual files
# This prevents Make from confusing them with files of the same name
.PHONY: all directories clean clean-all run test init-db docs help
