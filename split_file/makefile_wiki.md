# Matrix Multiplication Makefile Wiki

## Table of Contents
- [Quick Start](#quick-start)
- [Build Targets](#build-targets)
- [Testing Commands](#testing-commands)
- [Development Tools](#development-tools)
- [Advanced Usage](#advanced-usage)
- [Troubleshooting](#troubleshooting)
- [Configuration](#configuration)

## Quick Start

### Prerequisites
```bash
# Required
sudo apt-get install gcc make

# Optional but recommended
sudo apt-get install cppcheck valgrind clang-format linux-tools-generic
```

### Basic Build and Run
```bash
# Clone or navigate to the project directory
cd split_file/

# Build the project (optimized version)
make

# Run basic benchmark
./bin/matmul_benchmark

# Run with custom parameters
./bin/matmul_benchmark 512 3 5  # matrix_size warmup_runs test_runs
```

## Build Targets

### Primary Build Targets

| Target | Description | Output |
|--------|-------------|---------|
| `make` or `make all` | Build optimized release version | `bin/matmul_benchmark` |
| `make debug` | Build with debug symbols and no optimization | `bin/matmul_benchmark_debug` |
| `make perf` | Build with maximum performance optimizations | `bin/matmul_benchmark_perf` |

**Examples:**
```bash
# Standard optimized build
make all

# Debug build for development
make debug

# Maximum performance build
make perf
```

### Build Configuration Details

| Build Type | Compiler Flags | Use Case |
|------------|----------------|----------|
| **Release** | `-O3 -march=native -ffast-math` | Production benchmarking |
| **Debug** | `-g -O0 -DDEBUG` | Development and debugging |
| **Performance** | Release flags + `-funroll-loops -ftree-vectorize` | Maximum speed |

## Testing Commands

### Quick Testing
```bash
# Run verification tests (small matrices)
make verify

# Run full benchmark suite
make test
```

### Detailed Testing Examples
```bash
# Test different matrix sizes
make test
# This runs:
# - 64x64 matrices (2 warmup, 3 test runs)
# - 256x256 matrices (2 warmup, 3 test runs)  
# - 512x512 matrices (1 warmup, 2 test runs)

# Manual testing with specific parameters
./bin/matmul_benchmark 128 1 1    # Quick test
./bin/matmul_benchmark 1024 5 10  # Thorough benchmark
```

### Understanding Test Output
```
=================================================================
MODULAR MATRIX MULTIPLICATION BENCHMARK
Based on Golub & Van Loan 'Matrix Computations' Chapter 1
=================================================================
Matrix dimensions: 256 x 256
Warmup runs: 3, Test runs: 5
Total FLOPs per multiplication: 33554432

Verifying algorithm correctness...
  jik: PASS
  saxpy: PASS
  outer_product: PASS
  ikj: PASS
  kij: PASS
  ikj_inlined: PASS
  blocked: PASS

Running benchmarks...

=================================================================
PERFORMANCE RESULTS
=================================================================
Algorithm                 Time (s)      MFLOPS    Relative
-----------------------------------------------------------------
ijk (dot product)           0.125431       267.58      2.15x
jik                         0.124892       268.73      2.14x
saxpy                       0.089234       376.14      1.53x
outer product               0.078456       427.61      1.34x
ikj (modular)               0.063789       525.78      1.09x
kij                         0.064123       523.04      1.10x
ikj (inlined)               0.058432       574.32      1.00x
blocked (bs=32)             0.059821       560.67      1.02x
blocked (bs=64)             0.058891       569.83      1.01x
blocked (bs=128)            0.062341       538.23      1.07x
-----------------------------------------------------------------
```

## Development Tools

### Static Analysis
```bash
# Check code quality (requires cppcheck)
make check

# Sample output:
# Running static analysis...
# Checking main.c ...
# Checking matrix_utils.c ...
# [main.c:45]: (style) Variable 'result' is assigned but never used
```

### Memory Testing
```bash
# Debug build + memory leak detection (requires valgrind)
make memcheck

# Sample commands run:
# valgrind --leak-check=full --show-leak-kinds=all \
#          --track-origins=yes --verbose \
#          bin/matmul_benchmark_debug 64 1 1
```

### Performance Profiling
```bash
# Performance profiling (requires perf)
make profile

# This runs:
# perf record -g bin/matmul_benchmark_perf 256 1 3
# perf report  # Opens interactive report
```

### Code Formatting
```bash
# Format all source files (requires clang-format)
make format

# Formats according to project style guidelines
```

### Assembly Analysis
```bash
# Generate assembly files for optimization analysis
make assembly

# Output files created in asm/ directory:
# asm/main.s
# asm/matrix_utils.s
# asm/matmul_basic.s
# asm/matmul_optimized.s
# etc.
```

## Advanced Usage

### Custom Builds
```bash
# Build with custom compiler flags
make CFLAGS="-O2 -g -Wall" all

# Build with different compiler
make CC=clang all

# Build with additional libraries
make LDFLAGS="-lm -lrt -lpthread" all
```

### Parallel Builds
```bash
# Use multiple CPU cores for compilation
make -j4 all      # Use 4 cores
make -j$(nproc)   # Use all available cores
```

### Installation
```bash
# Install to system (requires sudo)
make install      # Installs to /usr/local/bin/

# Uninstall from system
make uninstall
```

### Cross-Platform Considerations
```bash
# For macOS (may need different flags)
make CC=clang LDFLAGS="-lm" all

# For older systems (without march=native)
make CFLAGS="-O3 -Wall -Wextra -std=c99" all
```

## Configuration

### Makefile Variables You Can Override

| Variable | Default | Description |
|----------|---------|-------------|
| `CC` | `gcc` | Compiler to use |
| `CFLAGS` | `-Wall -Wextra -O3 -std=c99 -march=native -ffast-math` | Compilation flags |
| `LDFLAGS` | `-lm -lrt` | Linking flags |
| `OBJ_DIR` | `obj` | Object files directory |
| `BIN_DIR` | `bin` | Executables directory |

### Examples of Custom Configuration
```bash
# Conservative optimization for compatibility
make CFLAGS="-O2 -Wall -std=c99" all

# Debug with extra checks
make CFLAGS="-g -O0 -Wall -Wextra -fsanitize=address" \
     LDFLAGS="-lm -lrt -fsanitize=address" debug

# Profile-guided optimization (advanced)
make CFLAGS="-O3 -fprofile-generate" all
./bin/matmul_benchmark 256 3 5
make clean
make CFLAGS="-O3 -fprofile-use" all
```

## Troubleshooting

### Common Issues and Solutions

#### Build Failures
```bash
# Issue: "gcc: command not found"
sudo apt-get install gcc

# Issue: "math.h: No such file"
sudo apt-get install build-essential

# Issue: "clock_gettime undefined"
# Solution: Ensure -lrt is in LDFLAGS (already included)
```

#### Missing Dependencies
```bash
# Optional tools not found warnings
# These are safe to ignore if you don't need the features:

# "cppcheck not found, skipping static analysis"
sudo apt-get install cppcheck

# "valgrind not found, skipping memory check"  
sudo apt-get install valgrind

# "clang-format not found, skipping code formatting"
sudo apt-get install clang-format

# "perf not found, skipping profiling"
sudo apt-get install linux-tools-generic
```

#### Runtime Issues
```bash
# Issue: Segmentation fault
# Solution: Use debug build and gdb
make debug
gdb ./bin/matmul_benchmark_debug
(gdb) run 64 1 1
(gdb) bt  # Show backtrace on crash

# Issue: Performance unexpectedly low
# Try performance build:
make perf
./bin/matmul_benchmark_perf

# Check CPU governor:
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
# Should be "performance" for benchmarking
```

#### Permission Issues
```bash
# Issue: Permission denied creating directories
# Solution: Ensure write permissions in current directory
chmod u+w .

# Issue: Cannot install (permission denied)
# Solution: Use sudo for system installation
sudo make install
```

### Getting Help
```bash
# Show all available targets and descriptions
make help

# Show current build configuration
make info

# Verbose build (see all commands)
make V=1 all
```

### Performance Tips

#### For Accurate Benchmarking
```bash
# 1. Use performance CPU governor
echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# 2. Disable CPU frequency scaling
sudo systemctl stop ondemand

# 3. Run on isolated CPU cores (advanced)
taskset -c 2,3 ./bin/matmul_benchmark_perf 1024 5 10

# 4. Use consistent test parameters
./bin/matmul_benchmark 512 5 10  # 5 warmup, 10 test runs
```

#### Understanding Results
- **MFLOPS**: Million Floating Point Operations Per Second (higher is better)
- **Relative**: Time relative to fastest algorithm (lower is better)  
- **Warmup runs**: Eliminate cold cache effects
- **Test runs**: Multiple measurements for statistical reliability

The benchmark compares different matrix multiplication algorithms showing the impact of:
- Loop ordering (cache locality)
- Function call overhead
- Block-based optimization
- Memory access patterns

This educational project demonstrates concepts from Golub & Van Loan's "Matrix Computations" textbook in a practical, measurable way.