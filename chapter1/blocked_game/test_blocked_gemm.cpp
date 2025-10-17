#include <iostream>
#include <cmath>
#include "blocked_gemm.h"

bool matrices_equal(const Matrix& A, const Matrix& B, double tolerance = 1e-10) {
    if (A.m != B.m || A.n != B.n) return false;
    
    for (int i = 0; i < A.m; i++) {
        for (int j = 0; j < A.n; j++) {
            if (std::abs(A(i, j) - B(i, j)) > tolerance) {
                std::cout << "Mismatch at (" << i << "," << j << "): "
                         << A(i, j) << " vs " << B(i, j) << "\n";
                return false;
            }
        }
    }
    return true;
}

bool test_gemm_implementation(void (*gemm_func)(const Matrix&, const Matrix&, Matrix&),
                              const std::string& name) {
    std::cout << "Testing " << name << "... ";
    
    // Test 1: Small matrix
    Matrix A1(3, 4);
    Matrix B1(4, 3);
    Matrix C1_expected(3, 3);
    Matrix C1_actual(3, 3);
    
    A1.fill_random();
    B1.fill_random();
    
    // Compute with ikj (known correct)
    gemm_ikj(A1, B1, C1_expected);
    
    // Compute with test function
    gemm_func(A1, B1, C1_actual);
    
    if (!matrices_equal(C1_expected, C1_actual)) {
        std::cout << "FAILED (small matrix)\n";
        return false;
    }
    
    // Test 2: Larger matrix with non-divisible dimensions
    Matrix A2(50, 47);
    Matrix B2(47, 53);
    Matrix C2_expected(50, 53);
    Matrix C2_actual(50, 53);
    
    A2.fill_random();
    B2.fill_random();
    
    gemm_ikj(A2, B2, C2_expected);
    gemm_func(A2, B2, C2_actual);
    
    if (!matrices_equal(C2_expected, C2_actual)) {
        std::cout << "FAILED (non-divisible dimensions)\n";
        return false;
    }
    
    std::cout << "PASSED ✓\n";
    return true;
}

int main() {
    std::cout << "==============================================\n";
    std::cout << "Testing Blocked GEMM Implementations\n";
    std::cout << "==============================================\n\n";
    
    bool all_passed = true;
    
    all_passed &= test_gemm_implementation(gemm_ikj, "ikj (control)");
    all_passed &= test_gemm_implementation(gemm_blocked_32, "blocked (block_size=32)");
    all_passed &= test_gemm_implementation(gemm_blocked_64, "blocked (block_size=64)");
    all_passed &= test_gemm_implementation(gemm_blocked_128, "blocked (block_size=128)");
    all_passed &= test_gemm_implementation(gemm_blocked_256, "blocked (block_size=256)");
    
    std::cout << "\n";
    if (all_passed) {
        std::cout << "✓ All tests passed!\n";
        std::cout << "Ready for benchmarking.\n";
    } else {
        std::cout << "✗ Some tests failed!\n";
        std::cout << "Fix implementations before benchmarking.\n";
    }
    
    return all_passed ? 0 : 1;
}
