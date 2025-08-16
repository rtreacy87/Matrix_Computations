#include "matrix_types.h"

// Timer utilities
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

Matrix* create_matrix(int rows, int cols) {
	Matrix *m = malloc(sizeof(Matrix));
	m->rows = rows;
	m->cols = cols;
	m->data = calloc(rows * cols, sizeof(double));
	return m;
}

void free_matrix(Matrix *m) {
	if (m) {
		free(m->data);
		free(m);
	}
}	

void int_random_matrix(Matrix *m){
	for (int i = 0; i < m->rows; i++) {
		m->data[i] = (double)rand() / RAND_MAX * 2.0-1.0; // Random values between -1.0 and 1.0
	}
}

void zero_matrix(Matrix *m) {
	memset(m->data, 0, m->rows * m->cols * sizeof(double));
}

void copy_matrix(Matrix *dest, Matrix *src) {
	memcpy(dest->data, src->data, src->rows * src->cols * sizeof(double));
}
