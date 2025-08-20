// ===========================================================================
// level1_blas.c - Level 1 BLAS operations (vector-vector)
// ===========================================================================
#include <math.h>

// Dot product: alpha = x^T * y (Algorithm from book)
double dot_product(double *x, double *y, int n) {
    double result = 0.0;
    for (int i = 0; i < n; i++) {
        result += x[i] * y[i];
    }
    return result;
}

// SAXPY: y = y + alpha*x (Algorithm from book)
void saxpy(double *y, double alpha, double *x, int n) {
    for (int i = 0; i < n; i++) {
        y[i] += alpha * x[i];
    }
}

// Vector 2-norm
double vector_norm(double *x, int n) {
    return sqrt(dot_product(x, x, n));
}
