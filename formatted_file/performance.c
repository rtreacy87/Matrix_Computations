// ===========================================================================
// performance.c - Performance measurement and analysis
// ===========================================================================
#include "matrix_types.h"
#include "matrix_utils.h"
#include "matmul_optimized.h"

PerfResult benchmark_algorithm(MatMulFunc func, const char* name, 
                              Matrix *A, Matrix *B, int warmup_runs, int test_runs) {
    int m = A->rows, r = A->cols, n = B->cols;
    Matrix *C = create_matrix(m, n);
    PerfResult result;
    strcpy(result.algorithm_name, name);
    
    // Warmup runs
    printf("  Warming up %s...\n", name);
    for (int run = 0; run < warmup_runs; run++) {
        zero_matrix(C);
        func(C, A, B);
    }
    
    // Timed runs
    printf("  Timing %s...\n", name);
    double total_time = 0.0;
    
    for (int run = 0; run < test_runs; run++) {
        zero_matrix(C);
        
        double start = get_time();
        func(C, A, B);
        double end = get_time();
        
        total_time += (end - start);
    }
    
    result.time_seconds = total_time / test_runs;
    result.flops = 2.0 * m * n * r;  // 2mnr flops as per Table 1.1.2
    result.mflops = result.flops / (result.time_seconds * 1e6);
    
    free_matrix(C);
    return result;
}

// Special benchmark for blocked algorithm
PerfResult benchmark_blocked(Matrix *A, Matrix *B, int block_size, 
                           int warmup_runs, int test_runs) {
    int m = A->rows, r = A->cols, n = B->cols;
    Matrix *C = create_matrix(m, n);
    PerfResult result;
    snprintf(result.algorithm_name, sizeof(result.algorithm_name), 
             "blocked (bs=%d)", block_size);
    
    // Warmup runs
    printf("  Warming up blocked (block_size=%d)...\n", block_size);
    for (int run = 0; run < warmup_runs; run++) {
        zero_matrix(C);
        matmul_blocked(C, A, B, block_size);
    }
    
    // Timed runs
    printf("  Timing blocked (block_size=%d)...\n", block_size);
    double total_time = 0.0;
    
    for (int run = 0; run < test_runs; run++) {
        zero_matrix(C);
        
        double start = get_time();
        matmul_blocked(C, A, B, block_size);
        double end = get_time();
        
        total_time += (end - start);
    }
    
    result.time_seconds = total_time / test_runs;
    result.flops = 2.0 * m * n * r;
    result.mflops = result.flops / (result.time_seconds * 1e6);
    
    free_matrix(C);
    return result;
}

void print_performance_results(PerfResult *results, int num_results) {
    printf("\n");
    printf("=================================================================\n");
    printf("PERFORMANCE RESULTS\n");
    printf("=================================================================\n");
    printf("%-25s %12s %12s %12s\n", "Algorithm", "Time (s)", "MFLOPS", "Relative");
    printf("-----------------------------------------------------------------\n");
    
    // Find fastest for relative comparison
    double fastest_time = results[0].time_seconds;
    for (int i = 1; i < num_results; i++) {
        if (results[i].time_seconds < fastest_time) {
            fastest_time = results[i].time_seconds;
        }
    }
    
    for (int i = 0; i < num_results; i++) {
        double relative = results[i].time_seconds / fastest_time;
        printf("%-25s %12.6f %12.2f %12.2fx\n", 
               results[i].algorithm_name, 
               results[i].time_seconds,
               results[i].mflops,
               relative);
    }
    printf("-----------------------------------------------------------------\n");
    printf("MFLOPS = Million Floating Point Operations Per Second\n");
    printf("Relative = Time relative to fastest algorithm\n");
    printf("\n");
}

