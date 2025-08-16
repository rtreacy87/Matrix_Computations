// ===========================================================================
// matmul_optimized.c - More optimized versions
// ===========================================================================
#include "matrix_types.h"

// Inlined version of ikj (no function calls)
void matmul_ikj_inlined(Matrix *C, Matrix *A, Matrix *B) {
    int m = A->rows, r = A->cols, n = B->cols;
    
    for (int i = 0; i < m; i++) {
        for (int k = 0; k < r; k++) {
            double a_ik = MAT(A, i, k);
            for (int j = 0; j < n; j++) {
                MAT(C, i, j) += a_ik * MAT(B, k, j);
            }
        }
    }
}

// Block-based matrix multiplication (Chapter 1.3 concepts)
void matmul_blocked(Matrix *C, Matrix *A, Matrix *B, int block_size) {
    int m = A->rows, r = A->cols, n = B->cols;
    
    for (int ii = 0; ii < m; ii += block_size) {
        for (int jj = 0; jj < n; jj += block_size) {
            for (int kk = 0; kk < r; kk += block_size) {
                
                // Block boundaries
                int i_end = (ii + block_size < m) ? ii + block_size : m;
                int j_end = (jj + block_size < n) ? jj + block_size : n;
                int k_end = (kk + block_size < r) ? kk + block_size : r;
                
                // Multiply blocks using ikj ordering
                for (int i = ii; i < i_end; i++) {
                    for (int k = kk; k < k_end; k++) {
                        double a_ik = MAT(A, i, k);
                        for (int j = jj; j < j_end; j++) {
                            MAT(C, i, j) += a_ik * MAT(B, k, j);
                        }
                    }
                }
            }
        }
    }
}
