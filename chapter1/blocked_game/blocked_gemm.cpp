#include "blocked_gemm.h"
#include <algorithm>

// ============================================================================
// CONTROL: ikj ordering (best unblocked from previous experiments)
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
// BLOCKED MATRIX MULTIPLICATION
// Based on Golub & Van Loan Section 1.3.5
// 
// Algorithm: Process C = C + AB in blocks
// for each block row i of A:
//     for each block column j of B:
//         for each block k:
//             C_ij = C_ij + A_ik * B_kj  (block multiplication)
//
// The key insight: Keep working sets in cache by processing blocks
// ============================================================================
void inner_block_inner_loop(const Matrix& A, const Matrix& B, Matrix& C,
                             int i, 
                             int k, 
                             int jj, 
                             int j_max) {
    for (int j = jj; j < j_max; j++) {
        C(i, j) += A(i, k) * B(k, j);
    }
}

void inner_block_middle_loop(const Matrix& A, const Matrix& B, Matrix& C,
                             int i,
                             int kk,
                             int k_max,
                             int j_max,
                             int jj){
    for (int k = kk; k < k_max; k++) {
        inner_block_inner_loop(A, B, C, i, k, jj, j_max);
    }
}

void inner_block_outer_loop(const Matrix& A, const Matrix& B, Matrix& C,
                            int ii,
                            int kk,
                            int jj,
                            int i_max,
                            int j_max,
                            int k_max) {
    for (int i = ii; i < i_max; i++) {
        inner_block_middle_loop(A, B, C, i, kk, k_max, j_max, jj);
    }
}

void outer_block_inner_loop(const Matrix& A, const Matrix& B, Matrix& C,
                            int ii,
                            int jj,
                            int block_size,
                            int m,
                            int n,
                            int r) {

    for (int kk = 0; kk < r; kk += block_size) {   // Block column of A / row of B
        
        // Determine actual block sizes (handle edge cases)
        int i_max = std::min(ii + block_size, m);
        int j_max = std::min(jj + block_size, n);
        int k_max = std::min(kk + block_size, r);
        
        // Inner loops: multiply the current blocks
        // C(ii:i_max, jj:j_max) += A(ii:i_max, kk:k_max) * B(kk:k_max, jj:j_max)
        inner_block_outer_loop(A, B, C, ii, kk, jj, i_max, j_max, k_max);
    }

}

void outer_block_middle_loop(const Matrix& A, const Matrix& B, Matrix& C,
                             int ii,
                             int block_size,
                             int m,
                             int n,
                             int r) {
  for (int jj = 0; jj < n; jj += block_size) {       // Block column of C/B
        outer_block_inner_loop(A, B, C, ii, jj, block_size, m, n, r);
  }
}

void outer_block_outer_loop(const Matrix& A, const Matrix& B, Matrix& C,
                             int block_size,
                             int m,
                             int n,
                             int r) {
    for (int ii = 0; ii < m; ii += block_size) {           // Block row of C/A
        outer_block_middle_loop(A, B, C, ii, block_size, m, n, r);
    }
}

void gemm_blocked(const Matrix& A, const Matrix& B, Matrix& C, int block_size) {
    const int m = A.m;
    const int n = B.n;
    const int r = A.n;  // = B.m
    outer_block_outer_loop(A, B, C, block_size, m, n, r);
}

// ============================================================================
// Convenience wrappers for testing different block sizes
// ============================================================================
void gemm_blocked_32(const Matrix& A, const Matrix& B, Matrix& C) {
    gemm_blocked(A, B, C, 32);
}

void gemm_blocked_64(const Matrix& A, const Matrix& B, Matrix& C) {
    gemm_blocked(A, B, C, 64);
}

void gemm_blocked_128(const Matrix& A, const Matrix& B, Matrix& C) {
    gemm_blocked(A, B, C, 128);
}

void gemm_blocked_256(const Matrix& A, const Matrix& B, Matrix& C) {
    gemm_blocked(A, B, C, 256);
}
