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

The framework is split into three files for easy experimentation:

```
your_project/
├── gaxpy.h          # Matrix class and function declarations
├── gaxpy.cpp        # Your gaxpy implementations (add new ones here!)
└── main.cpp         # Timer and benchmarking framework
```

**Benefits of this structure:**
- **gaxpy.cpp** contains only implementations - easy to add and modify
- **main.cpp** contains the testing framework - rarely needs changes
- **gaxpy.h** defines the interface - add declarations when you add new implementations

## Quick Start

### Compilation

```bash
# Basic compilation
g++ -std=c++17 -O3 -o gaxpy_test main.cpp gaxpy.cpp

# With CPU-specific optimizations (recommended)
g++ -std=c++17 -O3 -march=native -o gaxpy_test main.cpp gaxpy.cpp
```

**Compiler flags explained:**
- `-std=c++17`: Use C++17 standard features
- `-O3`: Maximum optimization level (critical for fair comparison)
- `-march=native`: Optimize for your specific CPU architecture
- `-o gaxpy_test`: Name the output executable

### Running

```bash
./gaxpy_test
```

## Framework Components

### File: gaxpy.h (Header)

This file contains:
- **Matrix class definition** with row-major storage
- **Function declarations** for all gaxpy implementations

**You edit this when:** Adding a new gaxpy implementation (add its declaration)

### File: gaxpy.cpp (Implementations)

This file contains all gaxpy implementations. **This is where you'll spend most of your time!**

**Current implementations:**
- `gaxpy_row_oriented()` - Processes matrix row-by-row
- `gaxpy_column_oriented()` - Processes matrix column-by-column

**You edit this when:** Adding or modifying gaxpy implementations

### File: main.cpp (Testing Framework)

This file contains:
- **Timer class** for accurate measurements
- **benchmark_gaxpy()** function for testing
- **main()** function that runs all tests

**You edit this when:** Adding new benchmarks or changing test parameters

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

**Storage layout:** Element (i,j) is stored at index `i * n + j` in the data vector (row-major order).

**Methods:**
- `Matrix(rows, cols)`: Constructor - creates an m×n matrix
- `operator()(i, j)`: Access element at row i, column j (0-indexed)
- `fill_random()`: Fills matrix with random values between -1.0 and 1.0

**Why row-major?** C and C++ store multi-dimensional arrays in row-major order, meaning consecutive elements in a row are adjacent in memory.

### Timer Class (main.cpp)

```cpp
class Timer {
    void start();
    double elapsed_ms();
};
```

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

### Benchmark Function (main.cpp)

```cpp
double benchmark_gaxpy(
    void (*gaxpy_func)(const Matrix&, const std::vector<double>&, std::vector<double>&),
    const Matrix& A,
    const std::vector<double>& x,
    std::vector<double>& y,
    int iterations)
```

**Purpose:** Accurately measures the performance of a gaxpy implementation.

**Parameters:**
- `gaxpy_func`: Pointer to the gaxpy function to test
- `A`: Input matrix
- `x`: Input vector
- `y`: Output vector (modified)
- `iterations`: Number of times to run the operation for averaging

**Returns:** Average time per iteration in milliseconds

**Process:**
1. Performs one warm-up run (to load caches, etc.)
2. Resets output vector y to zero
3. Runs the function `iterations` times while timing
4. Returns average time per iteration

## Understanding the Output

```
Matrix size: 1000 x 1000
Iterations: 100
  Row-oriented:        1.0139 ms
  Column-oriented:     0.6426 ms
  Speedup (row/col):   0.6338x
  Max difference:      0.0000

Matrix size: 5000 x 5000
Iterations: 100
  Row-oriented:       26.5430 ms
  Column-oriented:    40.3988 ms
  Speedup (row/col):   1.5220x
  Max difference:      0.0000
```

**Interpretation:**
- **Row-oriented time:** Average time for row-oriented implementation
- **Column-oriented time:** Average time for column-oriented implementation  
- **Speedup:** How many times faster row-oriented is than column-oriented (>1.0 means row is faster, <1.0 means column is faster)
- **Max difference:** Maximum error between the two results (should be ~0)

**Actual results show a crossover pattern:**
- **Small matrices (< ~2000×2000):** Column-oriented is FASTER (better register usage, sequential y access)
- **Large matrices (> ~2000×2000):** Row-oriented is FASTER (better cache locality for A)
- **Crossover point:** Depends on your CPU's cache size (typically L3 cache capacity)

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

### Step 3: Add benchmark in main.cpp

```cpp
// In main(), inside the size loop, after existing benchmarks:
std::vector<double> y_new(m, 0.0);
double time_new = benchmark_gaxpy(gaxpy_my_new_version, A, x, y_new, iterations);
std::cout << "  My new version: " << std::setw(10) << time_new << " ms\n";

// Verify correctness
double speedup_new = time_col / time_new;
std::cout << "  Speedup (new vs col): " << std::setw(8) << speedup_new << "x\n";

// Check against row-oriented implementation
double max_diff_new = 0.0;
for (int i = 0; i < m; i++) {
    max_diff_new = std::max(max_diff_new, std::abs(y_new[i] - y_row[i]));
}
std::cout << "  Max diff (new vs row): " << std::setw(10) << max_diff_new << "\n";
```

### Step 4: Recompile and run

```bash
g++ -std=c++17 -O3 -march=native -o gaxpy_test main.cpp gaxpy.cpp
./gaxpy_test
```

## Customization Options

### Changing Test Sizes (main.cpp)

Modify the `sizes` vector in `main()`:

```cpp
std::vector<std::pair<int, int>> sizes = {
    {50, 50},      // Small
    {200, 200},    // Medium
    {1000, 1000},  // Large
    {5000, 5000}   // Very large (requires more memory)
};
```

### Changing Iteration Count (main.cpp)

Adjust based on matrix size:

```cpp
int iterations = 100;  // Good for medium sizes
// For large matrices: iterations = 10;
// For small matrices: iterations = 1000;
```

### Testing Non-Square Matrices (main.cpp)

```cpp
std::vector<std::pair<int, int>> sizes = {
    {1000, 500},   // More rows than columns
    {500, 1000},   // More columns than rows
    {2000, 100}    // Very rectangular
};
```

## Performance Tips

### Compiler Optimization

Always use `-O3` for benchmarking:
```bash
# Good
g++ -std=c++17 -O3 -o gaxpy_test main.cpp gaxpy.cpp

# Better (CPU-specific optimizations)
g++ -std=c++17 -O3 -march=native -o gaxpy_test main.cpp gaxpy.cpp

# Bad (no optimization - results meaningless)
g++ -std=c++17 -o gaxpy_test main.cpp gaxpy.cpp
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

❌ **Forgetting -O3 flag** → Results don't reflect optimized performance

❌ **Forgetting to recompile** → Testing old code after changes

❌ **Too few iterations** → Timing noise dominates measurement

❌ **Not verifying correctness** → Fast but wrong implementation

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

**Add benchmark to main.cpp:**
```cpp
std::vector<double> y_blocked(m, 0.0);
double time_blocked = benchmark_gaxpy(gaxpy_blocked, A, x, y_blocked, iterations);
std::cout << "  Blocked version: " << std::setw(10) << time_blocked << " ms\n";
```

**Goal:** Maintain good cache locality for matrices of any size by processing in blocks that fit in cache.

**Expected result:** 
- Should match column-oriented for small matrices
- Should beat both versions for large matrices

**Tuning:** Try different block sizes (16, 32, 64, 128) to find optimal value for your CPU.

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

**Goal:** Reduce loop overhead and enable instruction-level parallelism.

**Expected result:** Most effective for small-to-medium matrices where instruction scheduling matters. May not help for very large matrices where memory bandwidth is the bottleneck.

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

1. **Write implementation** in `gaxpy.cpp`
2. **Add declaration** in `gaxpy.h`
3. **Add benchmark** in `main.cpp`
4. **Compile:** `g++ -std=c++17 -O3 -march=native -o gaxpy_test main.cpp gaxpy.cpp`
5. **Run:** `./gaxpy_test`
6. **Analyze results** and iterate!

## Further Reading

- Golub & Van Loan, "Matrix Computations" - Section 1.1 (Basic Algorithms) and Section 1.5 (Memory Hierarchy Effects)
- BLAS (Basic Linear Algebra Subprograms) documentation - see how production libraries handle these trade-offs
- "What Every Programmer Should Know About Memory" by Ulrich Drepper - deep dive into cache hierarchies
- Your CPU's specification sheet - find your L1/L2/L3 cache sizes to predict crossover points

## Key Takeaways

1. **No algorithm is universally best** - performance depends on problem size and hardware
2. **Cache capacity matters more than you think** - the crossover point reveals your cache size
3. **Theory vs practice** - always benchmark! Intuition about "row-major" access can be misleading
4. **Multiple factors interact** - matrix storage, vector access, register usage, and cache all matter
5. **Real libraries use adaptive algorithms** - BLAS implementations choose strategies based on matrix size

The goal of this framework is to help you develop intuition through experimentation. Happy benchmarking!
