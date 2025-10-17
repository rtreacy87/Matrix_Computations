#include <iostream>
#include <iomanip>
#include <vector>
#include "blocked_gemm.h"
#include "../src/matrix_utils.h"

// Benchmark a GEMM implementation
double benchmark_gemm(void (*gemm_func)(const Matrix&, const Matrix&, Matrix&),
                     int m, int n, int r, int iterations) {
    Matrix A(m, r);
    Matrix B(r, n);
    Matrix C(m, n);
    
    A.fill_random();
    B.fill_random();
    
    Timer timer;
    
    // Warm-up
    for(auto& v : C.data) v = 0.0;
    gemm_func(A, B, C);
    
    // Timed runs
    timer.start();
    for (int iter = 0; iter < iterations; iter++) {
        for(auto& v : C.data) v = 0.0;
        gemm_func(A, B, C);
    }
    double total_time = timer.elapsed_ms();
    
    return total_time / iterations;
}

void compare_gemm(void (*gemm1)(const Matrix&, const Matrix&, Matrix&),
                  void (*gemm2)(const Matrix&, const Matrix&, Matrix&),
                  const std::string& name1,
                  const std::string& name2,
                  int size,
                  int iterations) {
    
    std::cout << "Matrix size: " << size << "×" << size << "\n";
    
    double time1 = benchmark_gemm(gemm1, size, size, size, iterations);
    double time2 = benchmark_gemm(gemm2, size, size, size, iterations);
    
    double speedup = time1 / time2;  // >1 means gemm2 is faster
    
    std::cout << "  " << std::left << std::setw(30) << (name1 + ":") 
              << std::right << std::setw(10) << std::fixed << std::setprecision(3) 
              << time1 << " ms\n";
    std::cout << "  " << std::left << std::setw(30) << (name2 + ":") 
              << std::right << std::setw(10) << std::fixed << std::setprecision(3) 
              << time2 << " ms\n";
    std::cout << "  Speedup: " << std::fixed << std::setprecision(3) << speedup << "x";
    
    if (speedup > 1.05) {
        std::cout << " (" << name2 << " is FASTER) ⭐\n";
    } else if (speedup < 0.95) {
        std::cout << " (" << name1 << " is FASTER)\n";
    } else {
        std::cout << " (similar performance)\n";
    }
    std::cout << "\n";
}

int main() {
    std::cout << "================================================================\n";
    std::cout << "BLOCKED MATRIX MULTIPLICATION BENCHMARK\n";
    std::cout << "================================================================\n\n";
    
    std::cout << "Control: ikj (best unblocked from previous experiments)\n";
    std::cout << "Test: Blocked algorithm from Golub & Van Loan Section 1.3.5\n\n";
    
    std::cout << "Goal: Determine if blocking provides speedup over best unblocked\n";
    std::cout << "Expected: Blocking should win for large matrices (cache effects)\n\n";
    
    std::cout << "================================================================\n\n";
    
    // Test different matrix sizes
    std::vector<int> sizes = {100, 200, 400, 800, 1000};
    std::vector<int> iters = {100, 50, 20, 10, 5};
    
    std::cout << "EXPERIMENT 1: Find optimal block size at different matrix sizes\n";
    std::cout << "----------------------------------------------------------------\n\n";
    
    for (size_t i = 0; i < sizes.size(); i++) {
        std::cout << ">>> Testing at size " << sizes[i] << " <<<\n\n";
        
        compare_gemm(gemm_ikj, gemm_blocked_32, 
                    "ikj (unblocked)", "blocked-32", 
                    sizes[i], iters[i]);
        
        compare_gemm(gemm_ikj, gemm_blocked_64,
                    "ikj (unblocked)", "blocked-64",
                    sizes[i], iters[i]);
        
        compare_gemm(gemm_ikj, gemm_blocked_128,
                    "ikj (unblocked)", "blocked-128",
                    sizes[i], iters[i]);
        
        if (sizes[i] >= 400) {
            compare_gemm(gemm_ikj, gemm_blocked_256,
                        "ikj (unblocked)", "blocked-256",
                        sizes[i], iters[i]);
        }
        
        std::cout << "================================================================\n\n";
    }
    
    std::cout << "EXPERIMENT 2: Best block size vs unblocked across sizes\n";
    std::cout << "--------------------------------------------------------\n\n";
    
    std::cout << "Testing blocked-64 (likely optimal for L2 cache):\n\n";
    
    for (size_t i = 0; i < sizes.size(); i++) {
        compare_gemm(gemm_ikj, gemm_blocked_64,
                    "ikj", "blocked-64",
                    sizes[i], iters[i]);
    }
    
    std::cout << "================================================================\n";
    std::cout << "KEY QUESTIONS TO ANSWER:\n";
    std::cout << "================================================================\n\n";
    
    std::cout << "1. Does blocking beat unblocked ikj?\n";
    std::cout << "   → If yes, cache hierarchy is the bottleneck\n\n";
    
    std::cout << "2. Which block size is best?\n";
    std::cout << "   → Should correlate with L2 cache size\n\n";
    
    std::cout << "3. At what matrix size does blocking start winning?\n";
    std::cout << "   → Reveals when data exceeds cache capacity\n\n";
    
    std::cout << "4. Does speedup increase with matrix size?\n";
    std::cout << "   → Larger matrices = more cache misses = bigger blocking advantage\n\n";
    
    return 0;
}
