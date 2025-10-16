#include "gemm.h"
#include "../src/matrix_utils.h"

// ============================================================================
// ijk ordering - Dot product formulation
// Computes each element of C as dot product of A row with B column
// 
// Access patterns:
//   - A: row-by-row (good for row-major)
//   - B: column-by-column (BAD for row-major - jumps through memory)
//   - C: element-by-element
//
// Inner loop (k): dot product computation
// Middle loop (j): processes columns of B
// Outer loop (i): processes rows of A
// ============================================================================
void gemm_ijk(const Matrix& A, const Matrix& B, Matrix& C) {
    for (int i = 0; i < A.m; i++) {
        for (int j = 0; j < B.n; j++) {
            for (int k = 0; k < A.n; k++) {  // A.n = B.m = r
                C(i, j) += A(i, k) * B(k, j);
            }
        }
    }
}

// ============================================================================
// jik ordering - Matrix times vector formulation
// Similar to ijk but loops over columns first
//
// Access patterns:
//   - A: row-by-row (good for row-major)
//   - B: column-by-column (BAD for row-major)
//   - C: element-by-element
//
// Inner loop (k): dot product computation
// Middle loop (i): processes rows of A
// Outer loop (j): processes columns of B
// ============================================================================
void gemm_jik(const Matrix& A, const Matrix& B, Matrix& C) {
    for (int j = 0; j < B.n; j++) {
        for (int i = 0; i < A.m; i++) {
            for (int k = 0; k < A.n; k++) {
                C(i, j) += A(i, k) * B(k, j);
            }
        }
    }
}

// ============================================================================
// ikj ordering - Row-oriented gaxpy
// Processes rows: for each row of C, accumulate contributions
//
// Access patterns:
//   - A: row-by-row (GOOD for row-major)
//   - B: row-by-row (GOOD for row-major)  
//   - C: row-by-row (GOOD for row-major)
//
// Inner loop (j): saxpy operation across row
// Middle loop (k): processes columns of A / rows of B
// Outer loop (i): processes rows
//
// EXPECTED: BEST performance for C++ (all row-major access)
// ============================================================================
void gemm_ikj(const Matrix& A, const Matrix& B, Matrix& C) {
    for (int i = 0; i < A.m; i++) {
        for (int k = 0; k < A.n; k++) {
            for (int j = 0; j < B.n; j++) {
                C(i, j) += A(i, k) * B(k, j);
            }
        }
    }
}

// ============================================================================
// jki ordering - Column-oriented gaxpy
// Processes columns: for each column of C, accumulate contributions
//
// Access patterns:
//   - A: column-by-column (BAD for row-major)
//   - B: column-by-column (BAD for row-major)
//   - C: column-by-column (BAD for row-major)
//
// Inner loop (i): saxpy operation down column
// Middle loop (k): processes columns of A / rows of B  
// Outer loop (j): processes columns
//
// EXPECTED: WORST performance for C++ (all column-major access)
// EXPECTED: BEST performance for Fortran (column-major storage)
// ============================================================================
void gemm_jki(const Matrix& A, const Matrix& B, Matrix& C) {
    for (int j = 0; j < B.n; j++) {
        for (int k = 0; k < A.n; k++) {
            for (int i = 0; i < A.m; i++) {
                C(i, j) += A(i, k) * B(k, j);
            }
        }
    }
}

// ============================================================================
// kij ordering - Row-oriented outer product
// Accumulates outer products: C += A[:,k] * B[k,:]
//
// Access patterns:
//   - A: column-by-column then row-by-row (mixed)
//   - B: row-by-row (GOOD for row-major)
//   - C: row-by-row (GOOD for row-major)
//
// Inner loop (j): saxpy operation across row
// Middle loop (i): processes rows
// Outer loop (k): processes rank-1 updates
//
// EXPECTED: GOOD performance for C++ (mostly row-major access)
// ============================================================================
void gemm_kij(const Matrix& A, const Matrix& B, Matrix& C) {
    for (int k = 0; k < A.n; k++) {
        for (int i = 0; i < A.m; i++) {
            for (int j = 0; j < B.n; j++) {
                C(i, j) += A(i, k) * B(k, j);
            }
        }
    }
}

// ============================================================================
// kji ordering - Column-oriented outer product
// Accumulates outer products column-wise: C += A[:,k] * B[k,:]
//
// Access patterns:
//   - A: column-by-column (BAD for row-major)
//   - B: row-by-row then column-by-column (mixed)
//   - C: column-by-column (BAD for row-major)
//
// Inner loop (i): saxpy operation down column
// Middle loop (j): processes columns
// Outer loop (k): processes rank-1 updates
//
// EXPECTED: POOR performance for C++ (column-major access)
// ============================================================================
void gemm_kji(const Matrix& A, const Matrix& B, Matrix& C) {
    for (int k = 0; k < A.n; k++) {
        for (int j = 0; j < B.n; j++) {
            for (int i = 0; i < A.m; i++) {
                C(i, j) += A(i, k) * B(k, j);
            }
        }
    }
}
