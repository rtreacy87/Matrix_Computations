#include <iostream>
#include <iomanip>
#include <cmath>
#include "gemm.h"
#include "../src/matrix_utils.h"

// Benchmark a GEMM implementation
double benchmark_gemm(void (*gemm_func)(const Matrix&, const Matrix&, Matrix&),
                     int m, int n, int r, int iterations) {
    // Create matrices
    Matrix A(m, r);
    Matrix B(r, n);
    Matrix C(m, n);
    
    A.fill_random();
    B.fill_random();
    
    Timer timer;
    
    // Warm-up run
    for(auto& v : C.data) v = 0.0;
    gemm_func(A, B, C);
    
    // Timed runs
    timer.start();
    for (int iter = 0; iter < iterations; iter++) {
        // Reset C
        for(auto& v : C.data) v = 0.0;
        gemm_func(A, B, C);
    }
    double total_time = timer.elapsed_ms();
    
    return total_time / iterations;
}

// Compare two GEMM implementations
void compare_gemm(void (*gemm1)(const Matrix&, const Matrix&, Matrix&),
                  void (*gemm2)(const Matrix&, const Matrix&, Matrix&),
                  const std::string& name1,
                  const std::string& name2,
                  int size,
                  int iterations) {
    
    std::cout << "Matrix size: " << size << "×" << size << " (C = A*B)\n";
    std::cout << "Iterations: " << iterations << "\n";
    
    double time1 = benchmark_gemm(gemm1, size, size, size, iterations);
    double time2 = benchmark_gemm(gemm2, size, size, size, iterations);
    
    double speedup = time2 / time1;
    
    std::cout << "  " << std::left << std::setw(25) << (name1 + ":") 
              << std::right << std::fixed << std::setprecision(4) 
              << std::setw(10) << time1 << " ms\n";
    std::cout << "  " << std::left << std::setw(25) << (name2 + ":") 
              << std::right << std::setw(10) << time2 << " ms\n";
    std::cout << "  Speedup (" << name1 << "/" << name2 << "): " 
              << std::setw(8) << speedup << "x\n";
    
    // Verify correctness
    Matrix A(size, size), B(size, size);
    A.fill_random();
    B.fill_random();
    
    Matrix C1(size, size), C2(size, size);
    for(auto& v : C1.data) v = 0.0;
    for(auto& v : C2.data) v = 0.0;
    
    gemm1(A, B, C1);
    gemm2(A, B, C2);
    
    double max_diff = 0.0;
    for(size_t i = 0; i < C1.data.size(); i++) {
        max_diff = std::max(max_diff, std::abs(C1.data[i] - C2.data[i]));
    }
    
    std::cout << "  Max difference:  " << std::setw(10) << max_diff;
    if (max_diff > 1e-10) {
        std::cout << " ⚠️  WARNING";
    } else {
        std::cout << " ✓";
    }
    std::cout << "\n\n";
}

int main() {
    std::cout << "================================================================\n";
    std::cout << "Matrix-Matrix Multiplication: Six Loop Orderings\n";
    std::cout << "Testing C = C + A*B with different loop orders (i,j,k)\n";
    std::cout << "================================================================\n\n";
    
    std::cout << "PREDICTION for C++ (row-major storage):\n";
    std::cout << "  FASTEST:  ikj, kij (row-oriented - all sequential access)\n";
    std::cout << "  MEDIUM:   ijk, jik (mixed - some sequential, some strided)\n";
    std::cout << "  SLOWEST:  jki, kji (column-oriented - all strided access)\n\n";
    
    std::cout << "Expected speedup: 2-10x difference between best and worst!\n";
    std::cout << "Much more dramatic than gaxpy (which was ~1.5x)\n\n";
    
    std::cout << "================================================================\n\n";
    
    // Test with moderate size
    int test_size = 200;
    int iterations = 50;
    
    std::cout << "COMPARISON 1: Baseline (ijk) vs All Others\n";
    std::cout << "-------------------------------------------\n\n";
    
    compare_gemm(gemm_ijk, gemm_jik, 
                "ijk (dot product)", "jik (matrix×vector)", 
                test_size, iterations);
    
    compare_gemm(gemm_ijk, gemm_ikj, 
                "ijk (dot product)", "ikj (row gaxpy) ⭐", 
                test_size, iterations);
    
    compare_gemm(gemm_ijk, gemm_jki, 
                "ijk (dot product)", "jki (col gaxpy)", 
                test_size, iterations);
    
    compare_gemm(gemm_ijk, gemm_kij, 
                "ijk (dot product)", "kij (row outer) ⭐", 
                test_size, iterations);
    
    compare_gemm(gemm_ijk, gemm_kji, 
                "ijk (dot product)", "kji (col outer)", 
                test_size, iterations);
    
    std::cout << "\n";
    std::cout << "COMPARISON 2: Best Performers Head-to-Head\n";
    std::cout << "-------------------------------------------\n\n";
    
    compare_gemm(gemm_ikj, gemm_kij,
                "ikj (row gaxpy)", "kij (row outer prod)",
                test_size, iterations);
    
    std::cout << "\n";
    std::cout << "COMPARISON 3: Worst Performers Head-to-Head\n";
    std::cout << "--------------------------------------------\n\n";
    
    compare_gemm(gemm_jki, gemm_kji,
                "jki (col gaxpy)", "kji (col outer prod)",
                test_size, iterations);
    
    std::cout << "\n================================================================\n";
    std::cout << "SCALING TEST: How does size affect the gap?\n";
    std::cout << "================================================================\n\n";
    
    std::cout << "Testing best (ikj) vs worst (jki) at different sizes:\n\n";
    
    std::vector<int> sizes = {50, 100, 200, 400};
    std::vector<int> iters = {100, 100, 50, 20};
    
    for(size_t i = 0; i < sizes.size(); i++) {
        compare_gemm(gemm_ikj, gemm_jki,
                    "ikj (BEST)", "jki (WORST)",
                    sizes[i], iters[i]);
    }
    
    std::cout << "\n================================================================\n";
    std::cout << "KEY FINDINGS:\n";
    std::cout << "================================================================\n";
    std::cout << "⭐ = Best performers for C++ (row-major storage)\n\n";
    
    std::cout << "What you should see:\n";
    std::cout << "  1. ikj and kij are 2-5x FASTER than jki and kji\n";
    std::cout << "  2. The gap INCREASES with matrix size (cache effects)\n";
    std::cout << "  3. ijk and jik are somewhere in the middle\n\n";
    
    std::cout << "Why this matters:\n";
    std::cout << "  • Same arithmetic, vastly different performance\n";
    std::cout << "  • Memory access patterns dominate for large matrices\n";
    std::cout << "  • This is why BLAS libraries are carefully optimized\n";
    std::cout << "  • Cache-friendly algorithms can be 10x faster!\n\n";
    
    std::cout << "Table 1.1.1 from Golub & Van Loan in action!\n";
    std::cout << "================================================================\n";
    
    return 0;
}
