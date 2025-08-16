#ifndef MATRIX_TYPES_H
#define MATRIX_TYPES_H

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

// Matrix structure for easy handling
typedef struct {
    double *data;
    int rows;
    int cols;
} Matrix;

// Performance measurement structure
typedef struct {
    double time_seconds;
    double flops;
    double mflops;
    char algorithm_name[50];
} PerfResult;

// Access matrix element (row-major storage)
#define MAT(m, i, j) ((m)->data[(i) * (m)->cols + (j)])

// Function pointer type for matrix multiplication algorithms
typedef void (*MatMulFunc)(Matrix*, Matrix*, Matrix*);

#endif
