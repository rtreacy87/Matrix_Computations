#include "gaxpy.h"

// Row-oriented gaxpy: y = y + A*x
// Processes matrix row-by-row
// Best for row-major storage (C++, C, NumPy)
void gaxpy_row_oriented(const Matrix& A, 
                       const std::vector<double>& x, 
                       std::vector<double>& y) {
    for (int i = 0; i < A.m; i++) {
        for (int j = 0; j < A.n; j++) {
            y[i] += A(i, j) * x[j];
        }
    }
}

// Column-oriented gaxpy: y = y + A*x
// Processes matrix column-by-column
// Best for column-major storage (Fortran, MATLAB, Julia)
void gaxpy_column_oriented(const Matrix& A, 
                          const std::vector<double>& x, 
                          std::vector<double>& y) {
    for (int j = 0; j < A.n; j++) {
        for (int i = 0; i < A.m; i++) {
            y[i] += A(i, j) * x[j];
        }
    }
}

// Add your new implementations here!
// Example:
/*
void gaxpy_blocked(const Matrix& A, 
                  const std::vector<double>& x, 
                  std::vector<double>& y) {
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
*/
