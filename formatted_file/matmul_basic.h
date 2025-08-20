#ifndef MATMUL_BASIC_H
#define MATMUL_BASIC_H
#include "matrix_types.h"

void inner_dot_product(Matrix *C, Matrix *A, Matrix *B, int i, int j, int r);
void inner_saxpy_column(Matrix *C, Matrix *A, Matrix *B, int j, int k, int m);
void inner_saxpy_row(Matrix *C, Matrix *A, Matrix *B, int i, int k, int n);
void matmul_ijk(Matrix *C, Matrix *A, Matrix *B);
void matmul_jik(Matrix *C, Matrix *A, Matrix *B);
void matmul_saxpy(Matrix *C, Matrix *A, Matrix *B);
void matmul_ikj(Matrix *C, Matrix *A, Matrix *B);
void matmul_outer_product(Matrix *C, Matrix *A, Matrix *B);
void matmul_kij(Matrix *C, Matrix *A, Matrix *B);

#endif // MATMUL_BASIC_H
