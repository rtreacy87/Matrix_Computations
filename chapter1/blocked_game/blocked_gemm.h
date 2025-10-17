#ifndef BLOCKED_GEMM_H
#define BLOCKED_GEMM_H

#include "../src/matrix_utils.h"

// Control: Best unblocked version (ikj - row gaxpy)
void gemm_ikj(const Matrix& A, const Matrix& B, Matrix& C);

// Blocked version based on Golub & Van Loan Section 1.3.5
void gemm_blocked(const Matrix& A, const Matrix& B, Matrix& C, int block_size);

// Convenience wrappers for different block sizes
void gemm_blocked_32(const Matrix& A, const Matrix& B, Matrix& C);
void gemm_blocked_64(const Matrix& A, const Matrix& B, Matrix& C);
void gemm_blocked_128(const Matrix& A, const Matrix& B, Matrix& C);
void gemm_blocked_256(const Matrix& A, const Matrix& B, Matrix& C);

#endif // BLOCKED_GEMM_H
