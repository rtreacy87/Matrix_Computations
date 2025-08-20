// ===========================================================================
// matmul_basic.c - Basic matrix multiplication implementations
// ===========================================================================
#include "matrix_types.h"

// Inner loop kernels for different access patterns
void inner_dot_product(Matrix *C, Matrix *A, Matrix *B, int i, int j, int r) {
    for (int k = 0; k < r; k++) {
        MAT(C, i, j) += MAT(A, i, k) * MAT(B, k, j);
    }
}

void inner_saxpy_column(Matrix *C, Matrix *A, Matrix *B, int j, int k, int m) {
    double b_kj = MAT(B, k, j);
    for (int i = 0; i < m; i++) {
        MAT(C, i, j) += MAT(A, i, k) * b_kj;
    }
}

void inner_saxpy_row(Matrix *C, Matrix *A, Matrix *B, int i, int k, int n) {
    double a_ik = MAT(A, i, k);
    for (int j = 0; j < n; j++) {
        MAT(C, i, j) += a_ik * MAT(B, k, j);
    }
}

// Algorithm 1.1.5 (ijk Matrix Multiplication) - Dot Product Version
void matmul_ijk(Matrix *C, Matrix *A, Matrix *B) {
    int m = A->rows, r = A->cols, n = B->cols;
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            inner_dot_product(C, A, B, i, j, r);
        }
    }
}

// jik variant - different access pattern
void matmul_jik(Matrix *C, Matrix *A, Matrix *B) {
    int m = A->rows, r = A->cols, n = B->cols;
    
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            inner_dot_product(C, A, B, i, j, r);
        }
    }
}

// Algorithm 1.1.7 (Saxpy Matrix Multiplication)
void matmul_saxpy(Matrix *C, Matrix *A, Matrix *B) {
    int m = A->rows, r = A->cols, n = B->cols;
    
    for (int j = 0; j < n; j++) {
        for (int k = 0; k < r; k++) {
            inner_saxpy_column(C, A, B, j, k, m);
        }
    }
}

// ikj variant - cache-friendly version
void matmul_ikj(Matrix *C, Matrix *A, Matrix *B) {
    int m = A->rows, r = A->cols, n = B->cols;
    
    for (int i = 0; i < m; i++) {
        for (int k = 0; k < r; k++) {
            inner_saxpy_row(C, A, B, i, k, n);
        }
    }
}

// Algorithm 1.1.8 (Outer Product Matrix Multiplication)
void matmul_outer_product(Matrix *C, Matrix *A, Matrix *B) {
    int m = A->rows, r = A->cols, n = B->cols;
    
    for (int k = 0; k < r; k++) {
        // C = C + A(:,k) * B(k,:) - outer product update
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                MAT(C, i, j) += MAT(A, i, k) * MAT(B, k, j);
            }
        }
    }
}

// kij variant
void matmul_kij(Matrix *C, Matrix *A, Matrix *B) {
    int m = A->rows, r = A->cols, n = B->cols;
    
    for (int k = 0; k < r; k++) {
        for (int i = 0; i < m; i++) {
            inner_saxpy_row(C, A, B, i, k, n);
        }
    }
}
