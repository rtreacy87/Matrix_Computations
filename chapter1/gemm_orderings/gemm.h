#ifndef GEMM_H
#define GEMM_H

#include "../src/matrix_utils.h"

// General Matrix Multiply (GEMM): C = C + A*B
// All six loop orderings from Golub & Van Loan Table 1.1.1
// 
// Dimensions:
//   C is m×n
//   A is m×r  
//   B is r×n
//
// Operation: C(i,j) = C(i,j) + Σ(k=0 to r-1) A(i,k) * B(k,j)

// ============================================================================
// ijk ordering - Dot product formulation
// Inner loop: dot product of row of A with column of B
// A: accessed by row, B: accessed by column, C: random access
// ============================================================================
void gemm_ijk(const Matrix& A, const Matrix& B, Matrix& C);

// ============================================================================
// jik ordering - Matrix times vector formulation  
// Inner loop: dot product
// A: accessed by row, B: accessed by column, C: random access
// ============================================================================
void gemm_jik(const Matrix& A, const Matrix& B, Matrix& C);

// ============================================================================
// ikj ordering - Row-oriented gaxpy
// Inner loop: saxpy operation on rows
// A: accessed by row, B: accessed by row, C: accessed by row
// BEST for row-major storage (C/C++)
// ============================================================================
void gemm_ikj(const Matrix& A, const Matrix& B, Matrix& C);

// ============================================================================
// jki ordering - Column-oriented gaxpy
// Inner loop: saxpy operation on columns
// A: accessed by column, B: accessed by column, C: accessed by column
// BEST for column-major storage (Fortran/MATLAB)
// ============================================================================
void gemm_jki(const Matrix& A, const Matrix& B, Matrix& C);

// ============================================================================
// kij ordering - Row-oriented outer product
// Inner loop: saxpy on rows
// A: accessed by row, B: accessed by row, C: accessed by row
// BEST for row-major storage (C/C++)
// ============================================================================
void gemm_kij(const Matrix& A, const Matrix& B, Matrix& C);

// ============================================================================
// kji ordering - Column-oriented outer product
// Inner loop: saxpy on columns
// A: accessed by column, B: accessed by column, C: accessed by column
// BEST for column-major storage (Fortran/MATLAB)
// ============================================================================
void gemm_kji(const Matrix& A, const Matrix& B, Matrix& C);

#endif // GEMM_H
