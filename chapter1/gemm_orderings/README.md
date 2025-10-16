# Matrix-Matrix Multiplication: Six Loop Orderings

This project implements and benchmarks all six loop orderings for matrix-matrix multiplication (GEMM: General Matrix Multiply) as described in **Golub & Van Loan, Chapter 1.1, Table 1.1.1**.

## The Operation

**C = C + A*B**

Where:
- C is m×n
- A is m×r
- B is r×n

## The Six Orderings

| Loop Order | Inner Operation | Access Pattern | Expected Performance (C++) |
|------------|----------------|----------------|----------------------------|
| **ijk** | dot product | A: row, B: col, C: random | Medium |
| **jik** | matrix×vector | A: row, B: col, C: random | Medium |
| **ikj** ⭐ | row gaxpy | A: row, B: row, C: row | **BEST** |
| **jki** | column gaxpy | A: col, B: col, C: col | **WORST** |
| **kij** ⭐ | row outer product | A: row, B: row, C: row | **BEST** |
| **kji** | column outer product | A: col, B: col, C: col | **WORST** |

⭐ = Best for row-major storage (C/C++)

## Project Structure

```
chapter1/gemm_orderings/
├── gemm.h              # Function declarations for all six orderings
├── gemm.cpp            # Implementations with detailed comments
├── gemm_main.cpp       # Performance benchmarks
└── test_gemm.cpp       # Correctness tests
```

## Compilation

### From the `gemm_orderings/` directory:

**Compile tests:**
```bash
g++ -std=c++17 -O3 -I../src -o test_gemm test_gemm.cpp gemm.cpp
```

**Run tests:**
```bash
./test_gemm
```

**Compile benchmarks:**
```bash
g++ -std=c++17 -O3 -march=native -I../src -o gemm_bench gemm_main.cpp gemm.cpp
```

**Run benchmarks:**
```bash
./gemm_bench
```

## Expected Results

With row-major storage (C++):

**Performance ranking:**
1. **ikj and kij** - ~2-5x FASTER than worst (all row-major access)
2. **ijk and jik** - Medium performance (mixed access patterns)
3. **jki and kji** - ~2-5x SLOWER than best (all column-major access)

**Key insight:** The performance gap INCREASES with matrix size as cache effects dominate.

## What You'll Learn

1. **Same arithmetic, different performance** - All orderings compute the same result, but with dramatically different speeds

2. **Memory access patterns matter** - Sequential access (following storage order) is 2-5x faster than strided access

3. **Cache hierarchy dominates** - For large matrices, the gap between best and worst can be 10x or more

4. **Real-world relevance** - This is why BLAS libraries (used by NumPy, MATLAB, etc.) are so carefully optimized

## Comparison to Gaxpy

- **Gaxpy (2 loops):** ~1.5x difference between best and worst
- **GEMM (3 loops):** ~2-10x difference between best and worst

More loops = more opportunities for poor cache behavior!

## References

- Golub & Van Loan, "Matrix Computations", 4th Edition
- Chapter 1.1: Basic Algorithms and Notation
- Table 1.1.1: Matrix multiplication: loop orderings and properties
- Section 1.5: Vectorization and Locality

## Next Steps

After understanding loop orderings, explore:
1. **Blocked/tiled matrix multiplication** - Process in cache-sized chunks
2. **BLAS library comparison** - See how professional implementations perform
3. **Parallel implementations** - OpenMP or threading
4. **Strassen's algorithm** - O(n^2.807) instead of O(n^3)
