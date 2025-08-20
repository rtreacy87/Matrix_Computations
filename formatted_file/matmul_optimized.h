#ifndef MATMUL_OPTIMIZED_H
#define MATMUL_OPTIMIZED_H

#include "matrix_types.h"

void matmul_ikj_inlined(Matrix *C, Matrix *A, Matrix *B);
void matmul_blocked(Matrix *C, Matrix *A, Matrix *B, int block_size);

#endif // MATMUL_OPTIMIZED_H
