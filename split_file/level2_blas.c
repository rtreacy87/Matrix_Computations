// ===========================================================================
// level2_blas.c - Level 2 BLAS operations (matrix-vector)
// ===========================================================================
#include "matrix_types.h"

// Matrix-vector multiplication: y = y + A*x
void matrix_vector_mult(Matrix *A, double *x, double *y) {
    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->cols; j++) {
            y[i] += MAT(A, i, j) * x[j];
        }
    }
}

// GAXPY: Y = Y + A*X (generalized saxpy for matrices)
void gaxpy(Matrix *Y, Matrix *A, Matrix *X) {
    for (int j = 0; j < X->cols; j++) {
        for (int k = 0; k < A->cols; k++) {
            double x_kj = MAT(X, k, j);
            for (int i = 0; i < A->rows; i++) {
                MAT(Y, i, j) += MAT(A, i, k) * x_kj;
            }
        }
    }
}

// Outer product: A = A + x*y^T
void outer_product_update(Matrix *A, double *x, double *y) {
    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->cols; j++) {
            MAT(A, i, j) += x[i] * y[j];
        }
    }
}
