// ===========================================================================
// matmul_optimized.c - More optimized versions
// ===========================================================================
#include "matrix_types.h"


// Level 1 BLAS: Vector operations (innermost)
void saxpy_blas(double *y, const double *x, int n, double alpha) {
    // y = y + alpha * x  (single loop, no nesting visible here)
    for (int i = 0; i < n; i++) {
        y[i] += alpha * x[i];
    }
}

// Level 2 BLAS: Matrix-vector operations (middle level)  
void process_matrix_row(Matrix *C, Matrix *A, Matrix *B, int row_i) {
    // Process one row of result matrix (single loop, calls saxpy)
    for (int k = 0; k < A->cols; k++) {
        double a_ik = MAT(A, row_i, k);
        double *c_row = &MAT(C, row_i, 0);
        double *b_row = &MAT(B, k, 0);
        
        saxpy_blas(c_row, b_row, B->cols, a_ik);  // Call Level 1 function
    }
}

// Level 3 BLAS: Matrix-matrix operations (top level)
void matmul_ikj_inlined(Matrix *C, Matrix *A, Matrix *B) {
    // Main function with simple single loop (calls process_matrix_row)
    for (int i = 0; i < A->rows; i++) {
        process_matrix_row(C, A, B, i);  // Call Level 2 function
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
