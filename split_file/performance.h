// ===========================================================================
// performance.h - Performance measurement function declarations
// ===========================================================================
#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include "matrix_types.h"

// Function declarations
PerfResult benchmark_algorithm(MatMulFunc func, const char* name, 
                              Matrix *A, Matrix *B, int warmup_runs, int test_runs);

PerfResult benchmark_blocked(Matrix *A, Matrix *B, int block_size, 
                           int warmup_runs, int test_runs);

void print_performance_results(PerfResult *results, int num_results);

#endif // PERFORMANCE_H
