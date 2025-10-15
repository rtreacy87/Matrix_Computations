#include "gaxpy.h"


// void gaxpy_nested_for_loop(const Matrix& A, const std::vector<double>& x, std::vector<double>& y);
// void gaxpy_modular(const Matrix& A, const std::vector<double>& x, std::vector<double>& y);
// void gaxpy_functional(const Matrix& A, const std::vector<double>& x, std::vector<double>& y);
// void gaxpy_inline_hint(const Matrix& A, const std::vector<double>& x, std::vector<double>& y);


// ============================================================================
// Version 1: Nested for loop (baseline)
// Traditional implementation - everything in one function
// ============================================================================
void gaxpy_nested_for_loop(const Matrix& A, 
                           const std::vector<double>& x, 
                           std::vector<double>& y) {
    for (int i = 0; i < A.m; i++) {
        for (int j = 0; j < A.n; j++) {
            y[i] += A(i, j) * x[j];
        }
    }
}

// ============================================================================
// Version 2: Modular with helper function
// "Clean code" principle - extracted function with single responsibility
// Tests if compiler successfully inlines simple helper functions
// ============================================================================

// Helper function: Compute dot product of matrix row with vector
void compute_row_contribution(const Matrix& A, 
                              int row, 
                              const std::vector<double>& x, 
                              double& y_row) {
    for (int j = 0; j < A.n; j++) {
        y_row += A(row, j) * x[j];
    }
}

void gaxpy_modular(const Matrix& A, 
                   const std::vector<double>& x, 
                   std::vector<double>& y) {
    for (int i = 0; i < A.m; i++) {
        compute_row_contribution(A, i, x, y[i]);
    }
}

// ============================================================================
// Version 3: Functional style with lambda
// Modern C++ approach - tests lambda capture and inlining
// More flexible - lambda can capture context
// ============================================================================
void gaxpy_functional(const Matrix& A, 
                     const std::vector<double>& x, 
                     std::vector<double>& y) {
    // Lambda captures A and x by reference
    auto compute_dot_product = [&A, &x](int row) -> double {
        double result = 0.0;
        for (int j = 0; j < A.n; j++) {
            result += A(row, j) * x[j];
        }
        return result;
    };
    
    for (int i = 0; i < A.m; i++) {
        y[i] += compute_dot_product(i);
    }
}

// ============================================================================
// Version 4: Explicit inline hint
// Forces compiler to consider inlining (though modern compilers usually
// make better decisions on their own at -O3)
// ============================================================================

// Inline hint to compiler
inline void compute_row_contribution_inline(const Matrix& A, 
                                           int row, 
                                           const std::vector<double>& x, 
                                           double& y_row) {
    for (int j = 0; j < A.n; j++) {
        y_row += A(row, j) * x[j];
    }
}

void gaxpy_inline_hint(const Matrix& A, 
                       const std::vector<double>& x, 
                       std::vector<double>& y) {
    for (int i = 0; i < A.m; i++) {
        compute_row_contribution_inline(A, i, x, y[i]);
    }
}
