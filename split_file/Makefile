# ===========================================================================
# Makefile for Matrix Multiplication Benchmark Project
# Based on Golub & Van Loan 'Matrix Computations'
# ===========================================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c99 -march=native -ffast-math
DEBUG_FLAGS = -g -O0 -DDEBUG
LDFLAGS = -lm -lrt

# Project name and directories
PROJECT = matmul_benchmark
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SOURCES = main.c \
          matrix_utils.c \
          matmul_basic.c \
          matmul_optimized.c \
          performance.c \
          verification.c \
          level1_blas.c

# Header files  
HEADERS = matrix_types.h \
          matrix_utils.h \
          matmul_basic.h \
          matmul_optimized.h \
          performance.h \
          verification.h

# Object files
OBJECTS = $(SOURCES:%.c=$(OBJ_DIR)/%.o)

# Default target
all: directories $(BIN_DIR)/$(PROJECT)

# Create necessary directories
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Main executable
$(BIN_DIR)/$(PROJECT): $(OBJECTS)
	@echo "Linking $@..."
	@$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# Object file compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Debug build
debug: CFLAGS = $(DEBUG_FLAGS)
debug: directories $(BIN_DIR)/$(PROJECT)_debug

$(BIN_DIR)/$(PROJECT)_debug: $(OBJECTS)
	@echo "Linking debug version $@..."
	@$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Debug build complete: $@"

# Performance optimized build
perf: CFLAGS += -DNDEBUG -funroll-loops -ftree-vectorize
perf: directories $(BIN_DIR)/$(PROJECT)_perf

$(BIN_DIR)/$(PROJECT)_perf: $(OBJECTS)
	@echo "Linking performance version $@..."
	@$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Performance build complete: $@"

# Static analysis with cppcheck (if available)
check:
	@echo "Running static analysis..."
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c99 $(SOURCES); \
	else \
		echo "cppcheck not found, skipping static analysis"; \
	fi

# Format code with clang-format (if available)
format:
	@echo "Formatting code..."
	@if command -v clang-format >/dev/null 2>&1; then \
		clang-format -i $(SOURCES) $(HEADERS); \
		echo "Code formatted successfully"; \
	else \
		echo "clang-format not found, skipping code formatting"; \
	fi

# Run tests with different matrix sizes
test: $(BIN_DIR)/$(PROJECT)
	@echo "Running matrix multiplication benchmarks..."
	@echo "\n=== Small matrices (64x64) ==="
	@$(BIN_DIR)/$(PROJECT) 64 2 3
	@echo "\n=== Medium matrices (256x256) ==="
	@$(BIN_DIR)/$(PROJECT) 256 2 3
	@echo "\n=== Large matrices (512x512) ==="
	@$(BIN_DIR)/$(PROJECT) 512 1 2

# Quick test for verification only
verify: $(BIN_DIR)/$(PROJECT)
	@echo "Running verification tests..."
	@$(BIN_DIR)/$(PROJECT) 32 0 1

# Memory leak detection with valgrind (if available)
memcheck: debug
	@echo "Running memory leak detection..."
	@if command -v valgrind >/dev/null 2>&1; then \
		valgrind --leak-check=full --show-leak-kinds=all \
		--track-origins=yes --verbose \
		$(BIN_DIR)/$(PROJECT)_debug 64 1 1; \
	else \
		echo "valgrind not found, skipping memory check"; \
	fi

# Performance profiling with perf (if available)
profile: perf
	@echo "Running performance profiling..."
	@if command -v perf >/dev/null 2>&1; then \
		perf record -g $(BIN_DIR)/$(PROJECT)_perf 256 1 3; \
		perf report; \
	else \
		echo "perf not found, skipping profiling"; \
	fi

# Generate assembly output for optimization analysis
assembly: $(SOURCES)
	@echo "Generating assembly files..."
	@mkdir -p asm
	@for src in $(SOURCES); do \
		echo "Generating assembly for $$src..."; \
		$(CC) $(CFLAGS) -S $$src -o asm/$$(basename $$src .c).s; \
	done
	@echo "Assembly files generated in asm/ directory"

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(OBJ_DIR) $(BIN_DIR) asm
	@echo "Clean complete"

# Clean and rebuild
rebuild: clean all

# Install to system (requires sudo)
install: $(BIN_DIR)/$(PROJECT)
	@echo "Installing $(PROJECT)..."
	@sudo cp $(BIN_DIR)/$(PROJECT) /usr/local/bin/
	@echo "Installation complete"

# Uninstall from system (requires sudo)
uninstall:
	@echo "Uninstalling $(PROJECT)..."
	@sudo rm -f /usr/local/bin/$(PROJECT)
	@echo "Uninstall complete"

# Show help
help:
	@echo "Available targets:"
	@echo "  all       - Build optimized release version (default)"
	@echo "  debug     - Build debug version with symbols"
	@echo "  perf      - Build performance-optimized version"
	@echo "  test      - Run benchmark tests with different matrix sizes"
	@echo "  verify    - Run quick verification tests"
	@echo "  check     - Run static analysis (requires cppcheck)"
	@echo "  format    - Format code (requires clang-format)"
	@echo "  memcheck  - Check for memory leaks (requires valgrind)"
	@echo "  profile   - Run performance profiling (requires perf)"
	@echo "  assembly  - Generate assembly files for analysis"
	@echo "  clean     - Remove all build artifacts"
	@echo "  rebuild   - Clean and build"
	@echo "  install   - Install to /usr/local/bin (requires sudo)"
	@echo "  uninstall - Remove from /usr/local/bin (requires sudo)"
	@echo "  help      - Show this help message"

# Dependency tracking
-include $(OBJECTS:.o=.d)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) -MM -MF $@ -MT $(OBJ_DIR)/$*.o $<

# Mark targets as phony
.PHONY: all debug perf clean rebuild test verify check format memcheck profile assembly install uninstall help directories

# Special handling for specific dependencies
$(OBJ_DIR)/main.o: matrix_types.h matrix_utils.h matmul_basic.h matmul_optimized.h performance.h verification.h
$(OBJ_DIR)/matrix_utils.o: matrix_types.h matrix_utils.h
$(OBJ_DIR)/matmul_basic.o: matrix_types.h matmul_basic.h
$(OBJ_DIR)/matmul_optimized.o: matrix_types.h matmul_optimized.h
$(OBJ_DIR)/performance.o: matrix_types.h matrix_utils.h matmul_optimized.h performance.h
$(OBJ_DIR)/verification.o: matrix_types.h matrix_utils.h matmul_basic.h matmul_optimized.h verification.h
$(OBJ_DIR)/level1_blas.o: # No additional dependencies

# Print configuration
info:
	@echo "Build Configuration:"
	@echo "  Compiler: $(CC)"
	@echo "  CFLAGS: $(CFLAGS)"
	@echo "  LDFLAGS: $(LDFLAGS)"
	@echo "  Source files: $(SOURCES)"
	@echo "  Object files will be in: $(OBJ_DIR)/"
	@echo "  Executables will be in: $(BIN_DIR)/"
