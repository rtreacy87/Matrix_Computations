# Gaxpy Timing Framework User Guide

## Overview

This framework provides a simple, accurate way to benchmark different implementations of the gaxpy (generalized saxpy) operation: **y = y + Ax**, where A is a matrix, x and y are vectors.

The framework is designed to help you understand how memory access patterns affect performance in numerical linear algebra operations.

**Key lesson:** Performance is NOT simply "row-major code runs faster in C++." Real performance depends on:
- Matrix size relative to cache capacity
- Register allocation and instruction-level parallelism
- Hardware prefetching and cache line utilization
- The interaction between ALL memory accesses (matrix A, vectors x and y)

You'll discover that column-oriented code is often faster for small matrices, while row-oriented wins for large matrices!

## Project Structure

The framework uses a two-tier structure separating reusable utilities from specific projects:

```
chapter1/
├── src/                    # Reusable utilities (shared across projects)
│   ├── matrix_utils.h      # Matrix, Timer, BenchmarkConfig (header-only)
│   ├── benchmark.h         # Benchmarking framework declarations
│   └── benchmark.cpp       # Benchmarking framework implementation
│
└── row_v_col/              # Specific project: row vs column gaxpy comparison
    ├── gaxpy.h             # Project-specific function declarations
    ├── gaxpy.cpp           # Project-specific implementations
    ├── main.cpp            # Main program for this project
    ├── test_gaxpy.cpp      # Test suite for this project
    └── README.md           # Project documentation
```

**Benefits of this structure:**
- ✅ **Reusable code** - `src/` contains utilities used across multiple projects
- ✅ **Clean separation** - Each project (like `row_v_col/`) is self-contained
- ✅ **Easy to add projects** - Create new folders alongside `row_v_col/` that use `src/`
- ✅ **Shared improvements** - Fix a bug in `benchmark.cpp`, all projects benefit
- ✅ **Professional organization** - Mirrors real-world project structures

## Design Philosophy: Two-Tier Architecture

The framework separates **reusable utilities** from **project-specific code**:

### Reusable Layer (`src/`)

**matrix_utils.h** - Header-only utility library
- Matrix class with row-major storage
- Timer class for accurate measurements
- BenchmarkConfig struct for test parameters
- No .cpp file needed (all implementations inline)

**benchmark.h / benchmark.cpp** - Performance testing framework
- `benchmark_gaxpy()` - Test individual implementations
- `compare_implementations()` - Side-by-side comparisons
- Can be reused across any matrix computation project

**Why header-only for matrix_utils.h?**
- Simple, short methods that can be inlined
- Single-file include for convenience
- No separate compilation needed
- Perfect for utility classes

### Project Layer (`row_v_col/`)

**gaxpy.h / gaxpy.cpp** - Project-specific algorithms
- Function declarations and implementations
- Only contains gaxpy variants for this comparison

**main.cpp** - Project entry point
- Orchestrates comparisons specific to this project
- Includes headers from both layers

**test_gaxpy.cpp** - Project-specific tests
- Correctness verification for gaxpy implementations

### Dependency Structure

```
row_v_col/main.cpp
  ├─ includes ../src/benchmark.h
  │    └─ includes ../src/matrix_utils.h
  └─ includes gaxpy.h
       └─ includes ../src/matrix_utils.h

row_v_col/test_gaxpy.cpp
  └─ includes gaxpy.h
       └─ includes ../src/matrix_utils.h
```

**Benefits:**
- ✅ **Reusable framework** - `src/` can be shared across multiple chapter projects
- ✅ **Clean project code** - Each project folder is self-contained
- ✅ **Easy scaling** - Add `chapter1/blocked_gaxpy/`, `chapter1/saxpy/`, etc.
- ✅ **DRY principle** - Fix benchmark bugs once, all projects benefit
- ✅ **Professional structure** - Mirrors real software engineering practices

## Quick Start

### Compilation

Since the reusable utilities are in `../src/`, you need to specify the include path and source files:

```bash
# From within the row_v_col/ directory:

# Basic compilation
g++ -std=c++17 -O3 -I../src -o gaxpy_test main.cpp gaxpy.cpp ../src/benchmark.cpp

# With CPU-specific optimizations (recommended)
g++ -std=c++17 -O3 -march=native -I../src -o gaxpy_test main.cpp gaxpy.cpp ../src/benchmark.cpp
```

**Compiler flags explained:**
- `-std=c++17`: Use C++17 standard features
- `-O3`: Maximum optimization level (critical for fair comparison)
- `-march=native`: Optimize for your specific CPU architecture
- `-I../src`: Tell compiler to look for headers in the `../src/` directory
- `-o gaxpy_test`: Name the output executable

**Important notes:**
- The `-I../src` flag allows your code to use `#include "matrix_utils.h"` and `#include "benchmark.h"`
- You only need to compile `benchmark.cpp` (not `matrix_utils.h` since it's header-only)
- Adjust paths if you're compiling from a different directory

**Note:** You must compile with the `-I../src` flag and include `../src/benchmark.cpp`

**From the `row_v_col/` directory:**
```bash
g++ -std=c++17 -O3 -I../src -o gaxpy_test main.cpp gaxpy.cpp ../src/benchmark.cpp
```

**File compilation order doesn't matter:**
```bash
# All equivalent
g++ -std=c++17 -O3 -I../src -o gaxpy_test main.cpp gaxpy.cpp ../src/benchmark.cpp
g++ -std=c++17 -O3 -I../src -o gaxpy_test gaxpy.cpp ../src/benchmark.cpp main.cpp
g++ -std=c++17 -O3 -I../src -o gaxpy_test ../src/benchmark.cpp main.cpp gaxpy.cpp
```

The `-I../src` flag tells the compiler to look in the `src/` directory when it sees `#include "matrix_utils.h"` or `#include "benchmark.h"`.

### Running

```bash
./gaxpy_test
```

**Sample output:**
```
Performance Comparison: Row-oriented vs Column-oriented
====================================================

Matrix size: 1000 x 1000
Iterations: 100
  Row-oriented:          1.0139 ms
  Column-oriented:       0.6426 ms
  Speedup (Row-oriented/Column-oriented):   1.5781x
  Max difference:      0.0000 ✓
```

**Understanding speedup:**
- Speedup < 1.0: First implementation is faster
- Speedup > 1.0: Second implementation is faster
- Speedup = 1.0: Both implementations have equal performance

## Testing for Correctness

Before benchmarking, always verify your implementations are correct!

### Running the Test Suite

```bash
# From the row_v_col/ directory:

# Compile the test suite
g++ -std=c++17 -O3 -I../src -o test_gaxpy test_gaxpy.cpp gaxpy.cpp

# Run tests
./test_gaxpy
```

**Note:** Tests only need `gaxpy.cpp` (no `benchmark.cpp` needed). The `-I../src` flag allows test_gaxpy.cpp to include matrix_utils.h through gaxpy.h.

**Expected output:**
```
========================================
Gaxpy Implementation Test Suite
========================================

[Test: Matrix Class]
  ✓ Matrix dimensions correct
  ✓ Matrix data size correct
  ✓ Element (0,0) access
  ...

[Test: Known Values]
  ✓ Row-oriented: 2x2 known values
  ✓ Column-oriented: 2x2 known values

[Test: Implementation Equivalence]
  ✓ Equivalence for 1x1
  ✓ Equivalence for 5x5
  ...

========================================
Test Summary
========================================
Passed: 42
Failed: 0
Total:  42

✓ All tests passed!
```

### What the Test Suite Checks

1. **Matrix Class** - Basic functionality (construction, element access, random fill)
2. **Known Values** - Results match hand-calculated expected values
3. **Implementation Equivalence** - All implementations produce identical results
4. **Edge Cases** - Empty matrices, single row/column, zero values
5. **Accumulation** - Correctly handles non-zero initial y values (y += Ax)
6. **Identity Matrix** - I*x = x for all implementations

### Workflow: Test First, Then Benchmark

```bash
# 1. Write your new implementation in gaxpy.cpp
# 2. Add declaration to gaxpy.h
# 3. Test for correctness
g++ -std=c++17 -O3 -o test_gaxpy test_gaxpy.cpp gaxpy.cpp
./test_gaxpy

# 4. If tests pass, benchmark performance
g++ -std=c++17 -O3 -march=native -o gaxpy_test main.cpp gaxpy.cpp
./gaxpy_test
```

**Why test first?** A fast but incorrect implementation is useless! Always verify correctness before worrying about performance.

## Framework Components

### File: matrix_utils.h (in `src/`)

**Location:** `../src/matrix_utils.h` (relative to project directory)

**Header-only utility library** containing:
- **Matrix class** - Row-major storage with operator() access
- **Timer class** - High-resolution timing for benchmarks
- **BenchmarkConfig struct** - Groups shared test parameters

**You edit this when:** Rarely - only if adding new utility classes

**Why header-only?** All methods are simple enough to inline. No .cpp file needed!

### File: benchmark.h (in `src/`)

**Location:** `../src/benchmark.h` (relative to project directory)

This file contains:
- **Function declarations** for benchmarking functions
- Includes `matrix_utils.h` for Matrix, Timer, BenchmarkConfig

**You edit this when:** Rarely - only if adding new benchmarking features

### File: benchmark.cpp (in `src/`)

**Location:** `../src/benchmark.cpp` (relative to project directory)

This file contains:
- **benchmark_gaxpy()** implementation - Tests individual implementations
- **compare_implementations()** implementation - Side-by-side comparison

**You edit this when:** Rarely - only if modifying benchmarking behavior

### File: gaxpy.h (in `row_v_col/`)

**Location:** In your project directory

This file contains:
- **Function declarations** for gaxpy implementations
- Includes `matrix_utils.h` for Matrix type

**You edit this when:** Adding a new gaxpy implementation (add its declaration)

### File: gaxpy.cpp (in `row_v_col/`)

**Location:** In your project directory

This file contains all gaxpy implementations. **This is where you'll spend most of your time!**

**Current implementations:**
- `gaxpy_row_oriented()` - Processes matrix row-by-row
- `gaxpy_column_oriented()` - Processes matrix column-by-column

**You edit this when:** Adding or modifying gaxpy implementations

### File: main.cpp (in `row_v_col/`)

**Location:** In your project directory

This file contains:
- **main()** function that orchestrates all comparisons
- Very simple - just calls `compare_implementations()`

**You edit this when:** Adding new comparison tests or changing which implementations to compare

**Example - the entire main.cpp is just:**
```cpp
#include <iostream>
#include "gaxpy.h"
#include "benchmark.h"

int main() {
    std::vector<std::pair<int, int>> sizes = {
        {100, 100}, {1000, 1000}, {5000, 5000}
    };
    
    compare_implementations(
        gaxpy_row_oriented,
        gaxpy_column_oriented,
        "Row-oriented",
        "Column-oriented",
        sizes,
        100
    );
    
    return 0;
}
```

**That's it!** All the timing, comparison, and verification logic is in `../src/benchmark.cpp`.

### File: test_gaxpy.cpp (in `row_v_col/`)

**Location:** In your project directory

This file contains:
- Correctness tests for all implementations
- Test harness with assertion framework

**You edit this when:** Adding tests for new implementations

## Detailed Component Reference

### Matrix Class (gaxpy.h)

```cpp
class Matrix {
    int m, n;                    // Dimensions (rows, columns)
    std::vector<double> data;    // Storage in row-major order
    
    Matrix(int rows, int cols);
    double& operator()(int i, int j);
    void fill_random();
};
```

**Location:** Defined in `../src/matrix_utils.h`, included by `gaxpy.h`

**Storage layout:** Element (i,j) is stored at index `i * n + j` in the data vector (row-major order).

**Methods:**
- `Matrix(rows, cols)`: Constructor - creates an m×n matrix
- `operator()(i, j)`: Access element at row i, column j (0-indexed)
- `fill_random()`: Fills matrix with random values between -1.0 and 1.0

**Why row-major?** C and C++ store multi-dimensional arrays in row-major order, meaning consecutive elements in a row are adjacent in memory.

### Timer Class (matrix_utils.h in `src/`)

```cpp
class Timer {
    void start();
    double elapsed_ms();
};
```

**Location:** Defined in `../src/matrix_utils.h`

**Purpose:** Provides accurate timing using C++'s high-resolution clock.

**Methods:**
- `start()`: Records the current time as the starting point
- `elapsed_ms()`: Returns elapsed time in milliseconds since `start()` was called

**Usage example:**
```cpp
Timer timer;
timer.start();
// ... code to time ...
double time = timer.elapsed_ms();
std::cout << "Elapsed: " << time << " ms\n";
```

### BenchmarkConfig Struct (matrix_utils.h in `src/`)

```cpp
struct BenchmarkConfig {
    const Matrix& A;
    const std::vector<double>& x;
    int iterations;
};
```

**Location:** Defined in `../src/matrix_utils.h`

**Purpose:** Groups shared test parameters that remain constant across all implementations being compared.

**Design rationale:** Separates immutable inputs (A, x, iterations) from mutable outputs (y). This ensures all implementations are tested with identical inputs while allowing each to have its own output vector for verification.

**Fields:**
- `A`: The matrix to multiply (shared across all tests)
- `x`: The input vector (shared across all tests)
- `iterations`: Number of times to run for averaging (shared across all tests)

**Usage example:**
```cpp
Matrix A(100, 100);
A.fill_random();
std::vector<double> x(100);
// ... fill x ...

// Create config once
BenchmarkConfig config = {A, x, 100};

// Each implementation gets its own output vector
std::vector<double> y_row(100, 0.0);
std::vector<double> y_col(100, 0.0);

// Both use the same config - guaranteed identical inputs!
double time_row = benchmark_gaxpy(gaxpy_row_oriented, config, y_row);
double time_col = benchmark_gaxpy(gaxpy_column_oriented, config, y_col);
```

### Gaxpy Implementations (gaxpy.cpp)

#### Row-Oriented Gaxpy

```cpp
void gaxpy_row_oriented(const Matrix& A, 
                       const std::vector<double>& x, 
                       std::vector<double>& y)
```

**Algorithm:**
```
for each row i:
    for each column j:
        y[i] += A(i,j) * x[j]
```

**Memory access pattern:** Accesses matrix A row-by-row (horizontal traversal)

**Best for:** Row-major storage (C++, C, Python/NumPy) when matrix is large

#### Column-Oriented Gaxpy

```cpp
void gaxpy_column_oriented(const Matrix& A, 
                          const std::vector<double>& x, 
                          std::vector<double>& y)
```

**Algorithm:**
```
for each column j:
    for each row i:
        y[i] += A(i,j) * x[j]
```

**Memory access pattern:** Accesses matrix A column-by-column (vertical traversal)

**Best for:** Small matrices that fit in cache, or column-major storage (Fortran, MATLAB, Julia)

### Benchmark Function (benchmark.cpp)

```cpp
double benchmark_gaxpy(
    void (*gaxpy_func)(const Matrix&, const std::vector<double>&, std::vector<double>&),
    const BenchmarkConfig& config,
    std::vector<double>& y)
```

**Purpose:** Accurately measures the performance of a single gaxpy implementation.

**Parameters:**
- `gaxpy_func`: Pointer to the gaxpy function to test
- `config`: Shared test configuration (matrix A, vector x, iteration count)
- `y`: Output vector (modified by the function)

**Returns:** Average time per iteration in milliseconds

**Design note:** By separating config (shared inputs) from y (per-test output), this design ensures all implementations are tested with identical inputs while allowing proper verification of each result.

**Process:**
1. Performs one warm-up run (to load caches, etc.)
2. Resets output vector y to zero
3. Runs the function `config.iterations` times while timing
4. Returns average time per iteration

### Compare Implementations Function (benchmark.cpp)

```cpp
void compare_implementations(
    void (*gaxpy_func1)(const Matrix&, const std::vector<double>&, std::vector<double>&),
    void (*gaxpy_func2)(const Matrix&, const std::vector<double>&, std::vector<double>&),
    const std::string& name1,
    const std::string& name2,
    const std::vector<std::pair<int, int>>& sizes,
    int iterations = 100)
```

**Purpose:** Compares two gaxpy implementations side-by-side across multiple matrix sizes.

**Parameters:**
- `gaxpy_func1`: First implementation to test
- `gaxpy_func2`: Second implementation to test
- `name1`: Display name for first implementation (e.g., "Row-oriented")
- `name2`: Display name for second implementation (e.g., "Column-oriented")
- `sizes`: Vector of (m, n) pairs representing matrix dimensions to test
- `iterations`: Number of iterations for averaging (default: 100)

**Returns:** Nothing (prints results to console)

**What it does:**
- Tests both implementations on each matrix size
- Calculates speedup (time2 / time1)
- Verifies results match between implementations
- Provides clear, formatted output with warnings if results differ

**Usage example:**
```cpp
std::vector<std::pair<int, int>> sizes = {{100, 100}, {1000, 1000}};

compare_implementations(
    gaxpy_row_oriented,
    gaxpy_blocked,
    "Row-oriented",
    "Blocked",
    sizes,
    100
);
```

## Understanding the Output

The `compare_implementations()` function produces clear, formatted output:

```
Performance Comparison: Row-oriented vs Column-oriented
====================================================

Matrix size: 1000 x 1000
Iterations: 100
  Row-oriented:          1.0139 ms
  Column-oriented:       0.6426 ms
  Speedup (Row-oriented/Column-oriented):   0.6338x
  Max difference:      0.0000 ✓

Matrix size: 5000 x 5000
Iterations: 100
  Row-oriented:         26.5430 ms
  Column-oriented:      40.3988 ms
  Speedup (Row-oriented/Column-oriented):   1.5220x
  Max difference:      0.0000 ✓

Note: Speedup < 1.0 means first implementation is faster.
      Speedup > 1.0 means second implementation is faster.
```

**Interpretation:**
- **Implementation times:** Average time per iteration for each implementation
- **Speedup:** Ratio of time2/time1 (values < 1.0 mean first is faster, > 1.0 mean second is faster)
- **Max difference:** Maximum error between the two results (should be ~0)
- **✓ or ⚠️:** Visual indicator of whether results match (⚠️ appears if difference > 1e-10)

**Actual results show a crossover pattern:**
- **Small matrices (< ~2000×2000):** Column-oriented is FASTER (better register usage, sequential y access)
- **Large matrices (> ~2000×2000):** Row-oriented is FASTER (better cache locality for A)
- **Crossover point:** Depends on your CPU's cache size (typically L3 cache capacity)

## Design: BenchmarkConfig Struct

The framework uses a `BenchmarkConfig` struct to group shared test parameters:

```cpp
struct BenchmarkConfig {
    const Matrix& A;
    const std::vector<double>& x;
    int iterations;
};
```

**Why this design?**

This separates **shared inputs** (same for all implementations) from **per-test outputs** (unique to each implementation):

```cpp
// Define shared inputs once
BenchmarkConfig config = {A, x, 100};

// Each implementation gets its own output vector
std::vector<double> y_row(m, 0.0);
std::vector<double> y_col(m, 0.0);
std::vector<double> y_blocked(m, 0.0);

// All use identical inputs - fair comparison guaranteed!
double time_row = benchmark_gaxpy(gaxpy_row_oriented, config, y_row);
double time_col = benchmark_gaxpy(gaxpy_column_oriented, config, y_col);
double time_blocked = benchmark_gaxpy(gaxpy_blocked, config, y_blocked);
```

**Benefits:**
- ✅ **DRY principle** - A, x, iterations defined once
- ✅ **Error prevention** - Impossible to accidentally use different inputs
- ✅ **Clear intent** - Explicit about what's shared vs. what's per-test
- ✅ **Maintainable** - Adding shared parameters requires one change
- ✅ **Readable** - Clean function calls without repetition

## The Cache Effect Crossover

Real-world results show that **neither implementation is always better** - performance depends on matrix size relative to cache capacity.

### Typical Performance Pattern

| Matrix Size | Winner | Why |
|------------|--------|-----|
| 100×100 | **Column** | Everything fits in L2/L3 cache; register usage and sequential y access dominate |
| 500×500 | **Column** | Still mostly cache-resident; column version has better instruction-level parallelism |
| 1000×1000 | **Column** | Starting to feel cache pressure, but still winning |
| 2000×2000 | **Row** | **Crossover point!** Matrix exceeds cache, memory access patterns dominate |
| 5000×5000 | **Row** | Large cache miss penalty makes contiguous access critical |

### Why Column-Oriented Wins for Small Matrices

For matrices that fit in cache:

**Column-oriented advantages:**
1. `x[j]` stays in a register for the entire inner loop (no repeated loads)
2. Sequential writes to `y` array (good for write buffers)
3. Better instruction-level parallelism opportunities
4. Compiler can optimize inner loop more aggressively

**Row-oriented disadvantages:**
1. `x` vector accessed in scattered pattern across outer loop
2. More memory operations overall

### Why Row-Oriented Wins for Large Matrices

When the matrix exceeds cache capacity:

**Row-oriented advantages:**
1. Matrix A accessed contiguously (cache lines stay hot)
2. Hardware prefetcher can predict next cache line
3. Maximizes cache line reuse

**Column-oriented disadvantages:**
1. Matrix A accessed with stride `n` (jumps through memory)
2. Each column element likely causes a cache miss
3. Poor cache line utilization
4. Hardware prefetcher less effective

### Finding Your Crossover Point

The crossover point depends on:
- **L3 cache size:** Larger cache = later crossover
- **Matrix dimensions:** For m×n matrix, approximately `m × n × 8 bytes`
- **System load:** Other processes competing for cache

**Example calculation:**
- 2000×2000 matrix = 4,000,000 elements × 8 bytes = 32 MB
- If crossover at 2000×2000, your L3 cache is probably 16-32 MB
- 1000×1000 matrix = 8 MB (fits in typical L3 cache)

## Adding New Implementations

To add a new gaxpy implementation, you need to edit all three files:

### Step 1: Add declaration to gaxpy.h

```cpp
// Add this to the function declarations section
void gaxpy_my_new_version(const Matrix& A, 
                          const std::vector<double>& x, 
                          std::vector<double>& y);
```

### Step 2: Add implementation to gaxpy.cpp

```cpp
// Add this function to gaxpy.cpp
void gaxpy_my_new_version(const Matrix& A, 
                          const std::vector<double>& x, 
                          std::vector<double>& y) {
    // Your implementation here
    // Example: blocked version
    const int block_size = 64;
    for (int ii = 0; ii < A.m; ii += block_size) {
        for (int jj = 0; jj < A.n; jj += block_size) {
            int i_max = std::min(ii + block_size, A.m);
            int j_max = std::min(jj + block_size, A.n);
            for (int i = ii; i < i_max; i++) {
                for (int j = jj; j < j_max; j++) {
                    y[i] += A(i, j) * x[j];
                }
            }
        }
    }
}
```

### Step 3: Add tests to test_gaxpy.cpp

```cpp
// In test_implementation_equivalence(), add your new implementation:
std::vector<double> y_new(m, 0.0);
gaxpy_my_new_version(A, x, y_new);

std::string msg = "My new version matches for " + std::to_string(m) + "x" + std::to_string(n);
suite.assert_vectors_equal(y_new, y_row, 1e-10, msg);
```

### Step 4: Run tests to verify correctness

```bash
g++ -std=c++17 -O3 -o test_gaxpy test_gaxpy.cpp gaxpy.cpp
./test_gaxpy
```

### Step 5: Compare against existing implementations in main.cpp

The easiest way to benchmark your new implementation is to use the `compare_implementations()` function:

```cpp
// In main(), add a new comparison:
std::vector<std::pair<int, int>> sizes = {
    {100, 100}, {500, 500}, {1000, 1000}, {2000, 2000}, {5000, 5000}
};

// Compare your new version against row-oriented
compare_implementations(
    gaxpy_row_oriented,
    gaxpy_my_new_version,
    "Row-oriented",
    "My New Version",
    sizes,
    100  // iterations
);

// Or compare against column-oriented
compare_implementations(
    gaxpy_column_oriented,
    gaxpy_my_new_version,
    "Column-oriented",
    "My New Version",
    sizes,
    100
);
```

**Alternative - Manual benchmarking** (if you need more control):
```cpp
// In main(), inside the size loop:
std::vector<double> y_new(m, 0.0);
double time_new = benchmark_gaxpy(gaxpy_my_new_version, config, y_new);
std::cout << "  My new version: " << std::setw(10) << time_new << " ms\n";

// Calculate speedup vs column-oriented
double speedup_new = time_col / time_new;
std::cout << "  Speedup (new vs col): " << std::setw(8) << speedup_new << "x\n";
```

### Step 4: Recompile and run

```bash
g++ -std=c++17 -O3 -march=native -o gaxpy_test main.cpp gaxpy.cpp
./gaxpy_test
```

## Customization Options

### Changing Test Sizes

Modify the `sizes` vector in `main()`:

```cpp
std::vector<std::pair<int, int>> sizes = {
    {50, 50},      // Small
    {200, 200},    // Medium
    {1000, 1000},  // Large
    {5000, 5000}   // Very large (requires more memory)
};

compare_implementations(
    gaxpy_row_oriented,
    gaxpy_column_oriented,
    "Row-oriented",
    "Column-oriented",
    sizes,
    100
);
```

### Changing Iteration Count

Adjust based on matrix size:

```cpp
// For small matrices: more iterations for accurate timing
std::vector<std::pair<int, int>> small_sizes = {{100, 100}, {200, 200}};
compare_implementations(gaxpy_row_oriented, gaxpy_column_oriented,
                       "Row", "Column", small_sizes, 1000);

// For large matrices: fewer iterations (they take longer)
std::vector<std::pair<int, int>> large_sizes = {{5000, 5000}, {10000, 10000}};
compare_implementations(gaxpy_row_oriented, gaxpy_column_oriented,
                       "Row", "Column", large_sizes, 10);
```

### Testing Non-Square Matrices

```cpp
std::vector<std::pair<int, int>> rectangular = {
    {1000, 500},   // More rows than columns
    {500, 1000},   // More columns than rows
    {2000, 100}    // Very rectangular
};

compare_implementations(
    gaxpy_row_oriented,
    gaxpy_column_oriented,
    "Row-oriented",
    "Column-oriented",
    rectangular,
    100
);
```

### Multiple Comparisons in One Run

```cpp
int main() {
    std::vector<std::pair<int, int>> sizes = {
        {100, 100}, {1000, 1000}, {5000, 5000}
    };
    
    // Compare row vs column
    compare_implementations(gaxpy_row_oriented, gaxpy_column_oriented,
                           "Row", "Column", sizes, 100);
    
    // Compare row vs blocked
    compare_implementations(gaxpy_row_oriented, gaxpy_blocked,
                           "Row", "Blocked", sizes, 100);
    
    // Compare column vs blocked
    compare_implementations(gaxpy_column_oriented, gaxpy_blocked,
                           "Column", "Blocked", sizes, 100);
    
    return 0;
}
```

## Performance Tips

### Compiler Optimization

Always use `-O3` for benchmarking (commands from `row_v_col/` directory):

```bash
# Good
g++ -std=c++17 -O3 -I../src -o gaxpy_test main.cpp gaxpy.cpp ../src/benchmark.cpp

# Better (CPU-specific optimizations)
g++ -std=c++17 -O3 -march=native -I../src -o gaxpy_test main.cpp gaxpy.cpp ../src/benchmark.cpp

# Bad (no optimization - results meaningless)
g++ -std=c++17 -I../src -o gaxpy_test main.cpp gaxpy.cpp ../src/benchmark.cpp
```

### Fair Comparison

- Run multiple iterations and average
- Include warm-up runs to load caches
- Close other applications to reduce system noise
- Test with multiple matrix sizes
- Verify all implementations produce identical results

### System Considerations

- **CPU load:** Close heavy applications before benchmarking
- **Turbo boost:** May cause timing variability
- **Power management:** Disable power saving modes for consistent results
- **Memory:** Ensure matrices fit in RAM (avoid swapping)

## Common Pitfalls

❌ **Not testing before benchmarking** → Fast but wrong implementation

❌ **Forgetting -O3 flag** → Results don't reflect optimized performance

❌ **Forgetting to recompile** → Testing old code after changes

❌ **Too few iterations** → Timing noise dominates measurement

❌ **Not verifying correctness** → Even the benchmark can have subtle bugs

❌ **Testing only square matrices** → May miss performance characteristics

❌ **Running with other processes** → System noise affects measurements

## Example Experiments

### Experiment 1: Find Your Crossover Point

Test systematically to find where row-oriented becomes faster than column-oriented.

**Modify the sizes vector in main.cpp:**
```cpp
std::vector<std::pair<int, int>> sizes = {
    {500, 500},     // 2 MB
    {1000, 1000},   // 8 MB
    {1500, 1500},   // 18 MB
    {2000, 2000},   // 32 MB
    {2500, 2500},   // 50 MB
    {3000, 3000},   // 72 MB
};
```

**Goal:** Identify the matrix size where row-oriented becomes faster than column-oriented.

**Expected result:** You should see the speedup ratio transition from <1.0 (column wins) to >1.0 (row wins) at a specific size. This reveals your CPU's cache capacity!

### Experiment 2: Cache Effects with Different Shapes

Test rectangular matrices to understand how matrix shape affects the crossover.

**Modify the sizes vector in main.cpp:**
```cpp
std::vector<std::pair<int, int>> sizes = {
    {100, 10000},   // Very wide - x vector is large
    {10000, 100},   // Very tall - many rows
    {5000, 5000},   // Square - balanced dimensions
};
```

**Goal:** Determine if the crossover depends on total matrix size or specific dimensions.

**Expected result:** Total size (m × n) matters most, but shape can affect which version wins.

### Experiment 3: Blocked Implementation - Best of Both Worlds

Blocking processes the matrix in cache-sized chunks, maintaining good cache locality regardless of total matrix size.

**Add this implementation to gaxpy.cpp:**
```cpp
void gaxpy_blocked(const Matrix& A, 
                   const std::vector<double>& x, 
                   std::vector<double>& y) {
    const int block_size = 64;  // Experiment with different sizes
    for (int ii = 0; ii < A.m; ii += block_size) {
        for (int jj = 0; jj < A.n; jj += block_size) {
            int i_max = std::min(ii + block_size, A.m);
            int j_max = std::min(jj + block_size, A.n);
            for (int i = ii; i < i_max; i++) {
                for (int j = jj; j < j_max; j++) {
                    y[i] += A(i, j) * x[j];
                }
            }
        }
    }
}
```

**Add declaration to gaxpy.h:**
```cpp
void gaxpy_blocked(const Matrix& A, 
                   const std::vector<double>& x, 
                   std::vector<double>& y);
```

**Add comparison to main.cpp:**
```cpp
std::vector<std::pair<int, int>> sizes = {
    {100, 100}, {500, 500}, {1000, 1000}, {2000, 2000}, {5000, 5000}
};

// Compare blocked vs row-oriented
compare_implementations(
    gaxpy_row_oriented,
    gaxpy_blocked,
    "Row-oriented",
    "Blocked",
    sizes,
    100
);

// Compare blocked vs column-oriented
compare_implementations(
    gaxpy_column_oriented,
    gaxpy_blocked,
    "Column-oriented",
    "Blocked",
    sizes,
    100
);
```

**Goal:** Maintain good cache locality for matrices of any size by processing in blocks that fit in cache.

**Expected result:** 
- Should match or beat column-oriented for small matrices
- Should beat both versions for large matrices

**Tuning:** Try different block sizes (16, 32, 64, 128) to find optimal value for your CPU. Simply change the `block_size` constant and recompile.

### Experiment 4: Unrolled Loops for Instruction-Level Parallelism

Loop unrolling reduces loop overhead and enables the CPU to execute multiple operations in parallel.

**Add this implementation to gaxpy.cpp:**
```cpp
void gaxpy_unrolled(const Matrix& A, 
                    const std::vector<double>& x, 
                    std::vector<double>& y) {
    for (int i = 0; i < A.m; i++) {
        int j = 0;
        // Process 4 elements at a time
        for (; j + 3 < A.n; j += 4) {
            y[i] += A(i, j) * x[j];
            y[i] += A(i, j+1) * x[j+1];
            y[i] += A(i, j+2) * x[j+2];
            y[i] += A(i, j+3) * x[j+3];
        }
        // Handle remaining elements
        for (; j < A.n; j++) {
            y[i] += A(i, j) * x[j];
        }
    }
}
```

**Add comparison to main.cpp:**
```cpp
compare_implementations(
    gaxpy_row_oriented,
    gaxpy_unrolled,
    "Row-oriented",
    "Unrolled",
    sizes,
    100
);
```

**Goal:** Reduce loop overhead and enable instruction-level parallelism.

**Expected result:** Most effective for small-to-medium matrices where instruction scheduling matters. May not help for very large matrices where memory bandwidth is the bottleneck.

## Common Comparison Patterns

The `compare_implementations()` function makes it trivial to test different scenarios:

### Compare All Implementations Against a Baseline

```cpp
std::vector<std::pair<int, int>> sizes = {{100, 100}, {1000, 1000}, {5000, 5000}};

// Use row-oriented as baseline
compare_implementations(gaxpy_row_oriented, gaxpy_column_oriented, 
                       "Row-oriented", "Column-oriented", sizes, 100);
compare_implementations(gaxpy_row_oriented, gaxpy_blocked, 
                       "Row-oriented", "Blocked", sizes, 100);
compare_implementations(gaxpy_row_oriented, gaxpy_unrolled, 
                       "Row-oriented", "Unrolled", sizes, 100);
```

### Test Different Block Sizes

```cpp
// Create multiple blocked implementations with different block sizes
void gaxpy_blocked_32(const Matrix& A, const std::vector<double>& x, std::vector<double>& y) {
    // ... implementation with block_size = 32
}

void gaxpy_blocked_64(const Matrix& A, const std::vector<double>& x, std::vector<double>& y) {
    // ... implementation with block_size = 64
}

// Compare them
compare_implementations(gaxpy_blocked_32, gaxpy_blocked_64, 
                       "Blocked-32", "Blocked-64", sizes, 100);
```

### Focus on Specific Size Ranges

```cpp
// Test small matrices only
std::vector<std::pair<int, int>> small_sizes = {{50, 50}, {100, 100}, {200, 200}};
compare_implementations(gaxpy_row_oriented, gaxpy_column_oriented,
                       "Row", "Column", small_sizes, 1000);  // More iterations

// Test large matrices only  
std::vector<std::pair<int, int>> large_sizes = {{5000, 5000}, {10000, 10000}};
compare_implementations(gaxpy_row_oriented, gaxpy_column_oriented,
                       "Row", "Column", large_sizes, 10);  // Fewer iterations
```

### Test Rectangular Matrices

```cpp
std::vector<std::pair<int, int>> wide_matrices = {{100, 5000}, {200, 10000}};
compare_implementations(gaxpy_row_oriented, gaxpy_column_oriented,
                       "Row", "Column", wide_matrices, 100);

std::vector<std::pair<int, int>> tall_matrices = {{5000, 100}, {10000, 200}};
compare_implementations(gaxpy_row_oriented, gaxpy_column_oriented,
                       "Row", "Column", tall_matrices, 100);
```

## Technical Background

### Why Does Memory Order Matter?

Modern CPUs use cache hierarchies (L1, L2, L3), and performance depends on **both** the cache size and memory access pattern.

**For small data (fits in cache):**
- Register allocation and instruction-level parallelism dominate
- Access pattern matters less because everything is cache-resident
- Compiler optimizations have bigger impact

**For large data (exceeds cache):**
- Cache misses dominate performance
- Sequential access (following storage order) is critical
- Hardware prefetching becomes important

### Cache Hierarchy

**Typical cache sizes:**
- L1 cache: 32-64 KB per core (very fast, ~4 cycles)
- L2 cache: 256-512 KB per core (fast, ~12 cycles)
- L3 cache: 8-32 MB shared (slower, ~40 cycles)
- Main memory: GBs (very slow, ~200+ cycles)

**Key principle:** Minimize cache misses by accessing memory in patterns that match:
1. **Your data storage order** (row-major in C++)
2. **Your cache capacity** (use blocking for large matrices)

### Row-Major vs Column-Major

**Row-major (C/C++):** `A[i][j]` and `A[i][j+1]` are adjacent in memory

**Column-major (Fortran/MATLAB):** `A[i][j]` and `A[i+1][j]` are adjacent in memory

**Nuanced rule:** For small matrices, optimal access depends on register usage and instruction scheduling. For large matrices, access pattern should match storage order to minimize cache misses.

### Why Both Implementations Have Trade-offs

**Row-oriented gaxpy:**
- ✅ Reads matrix A contiguously (cache-friendly for large matrices)
- ❌ Accesses vector x in scattered pattern across iterations
- ✅ Single element y[i] stays in register during inner loop

**Column-oriented gaxpy:**
- ❌ Reads matrix A with stride n (cache-unfriendly for large matrices)
- ✅ Single element x[j] stays in register during entire inner loop
- ✅ Writes to y sequentially (write buffer friendly)

The winner depends on which factors dominate for your matrix size!

## Workflow Summary

**All commands assume you're in the `row_v_col/` directory:**

1. **Write implementation** in `gaxpy.cpp`
2. **Add declaration** in `gaxpy.h`
3. **Add test** in `test_gaxpy.cpp` (in the `test_implementation_equivalence` function)
4. **Compile and test:** 
   ```bash
   g++ -std=c++17 -O3 -I../src -o test_gaxpy test_gaxpy.cpp gaxpy.cpp
   ./test_gaxpy
   ```
5. **If tests pass, add comparison** in `main.cpp`:
   ```cpp
   compare_implementations(
       gaxpy_row_oriented,
       gaxpy_my_version,
       "Row-oriented",
       "My Version",
       sizes,
       100
   );
   ```
6. **Compile and benchmark:** 
   ```bash
   g++ -std=c++17 -O3 -march=native -I../src -o gaxpy_test main.cpp gaxpy.cpp ../src/benchmark.cpp
   ./gaxpy_test
   ```
7. **Analyze results** and iterate!

**Golden rule:** Test for correctness first, then optimize for performance!

**Key pattern:** Use `compare_implementations()` to easily compare any two implementations across multiple sizes with automatic verification.

**Remember:** The `-I../src` flag is crucial - it tells the compiler where to find the reusable utilities.

## Creating New Projects

The two-tier structure makes it easy to create new projects that reuse the benchmarking framework:

### Example: Creating a Blocked Gaxpy Project

```bash
# From chapter1/ directory
mkdir blocked_gaxpy
cd blocked_gaxpy
```

**Create your project files:**

**blocked_main.cpp:**
```cpp
#include <iostream>
#include "gaxpy.h"
#include "benchmark.h"

int main() {
    std::vector<std::pair<int, int>> sizes = {
        {100, 100}, {1000, 1000}, {5000, 5000}
    };
    
    compare_implementations(
        gaxpy_row_oriented,
        gaxpy_blocked_32,
        "Row-oriented",
        "Blocked-32",
        sizes,
        100
    );
    
    return 0;
}
```

**Compile:**
```bash
g++ -std=c++17 -O3 -march=native -I../src -o blocked_test blocked_main.cpp gaxpy.cpp ../src/benchmark.cpp
```

**Key insight:** You're reusing all of `src/` without copying any code! Any improvements to the benchmark framework benefit all projects.

### Your chapter1/ Directory Now Looks Like:

```
chapter1/
├── src/                    # Shared utilities
│   ├── matrix_utils.h
│   ├── benchmark.h
│   └── benchmark.cpp
│
├── row_v_col/             # Project 1
│   ├── gaxpy.h
│   ├── gaxpy.cpp
│   ├── main.cpp
│   └── test_gaxpy.cpp
│
└── blocked_gaxpy/         # Project 2 (NEW!)
    ├── gaxpy.h
    ├── gaxpy.cpp
    ├── blocked_main.cpp
    └── test_blocked.cpp
```

Both projects share the same `src/` utilities!

## Further Reading

- Golub & Van Loan, "Matrix Computations" - Section 1.1 (Basic Algorithms) and Section 1.5 (Memory Hierarchy Effects)
- BLAS (Basic Linear Algebra Subprograms) documentation - see how production libraries handle these trade-offs
- "What Every Programmer Should Know About Memory" by Ulrich Drepper - deep dive into cache hierarchies
- Your CPU's specification sheet - find your L1/L2/L3 cache sizes to predict crossover points

## Key Takeaways

1. **Test first, optimize second** - correctness is non-negotiable; use the test suite before benchmarking
2. **Two-tier architecture enables reuse** - Keep utilities in `src/`, projects in their own folders
3. **Use compare_implementations() for easy benchmarking** - comparing implementations is now just a few lines of code
4. **The -I../src flag is essential** - tells the compiler where to find reusable utilities
5. **No algorithm is universally best** - performance depends on problem size and hardware
6. **Cache capacity matters more than you think** - the crossover point reveals your cache size
7. **Theory vs practice** - always benchmark! Intuition about "row-major" access can be misleading
8. **Multiple factors interact** - matrix storage, vector access, register usage, and cache all matter
9. **Real libraries use adaptive algorithms** - BLAS implementations choose strategies based on matrix size

The goal of this framework is to help you develop intuition through experimentation. Happy benchmarking!

## Quick Reference

### Essential Commands (from `row_v_col/` directory)

**Compile tests:**
```bash
g++ -std=c++17 -O3 -I../src -o test_gaxpy test_gaxpy.cpp gaxpy.cpp
```

**Run tests:**
```bash
./test_gaxpy
```

**Compile benchmarks:**
```bash
g++ -std=c++17 -O3 -march=native -I../src -o gaxpy_test main.cpp gaxpy.cpp ../src/benchmark.cpp
```

**Run benchmarks:**
```bash
./gaxpy_test
```

### Key Points to Remember

- ✅ Always use `-I../src` to find the reusable utilities
- ✅ Include `../src/benchmark.cpp` when compiling benchmarks
- ✅ Tests don't need `benchmark.cpp`
- ✅ All three utilities (Matrix, Timer, BenchmarkConfig) are in `matrix_utils.h`
- ✅ `matrix_utils.h` is header-only (no .cpp file to compile)
- ✅ New projects can reuse `src/` without copying code
