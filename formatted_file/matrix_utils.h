#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include "matrix_types.h"

Matrix* create_matrix(int rows, int cols);
void free_matrix(Matrix *m);
void init_random_matrix(Matrix *m);
void zero_matrix(Matrix *m);
void copy_matrix(Matrix *dest, Matrix *src);
double get_time();

#endif // MATRIX_UTILS_H

