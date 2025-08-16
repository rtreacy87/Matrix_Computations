// ===========================================================================
// main.c - Main benchmark program
// ===========================================================================
#include "matrix_types.h"
#include "matrix_utils.h"
#include "matmul_basic.h"
#include "matmul_optimized.h"

int main(int argc, char *argv[]) {
    // Default parameters
    int matrix_size = 256;
    int warmup_runs = 3;
    int test_runs = 5;
    
    // Parse command line arguments
    if (argc > 1) matrix_size = atoi(argv[1]);
    if (argc > 2) warmup_runs = atoi(argv[2]);
    if (argc > 3) test_runs = atoi(argv[3]);
    
    printf("=================================================================\n");
    printf("MODULAR MATRIX MULTIPLICATION BENCHMARK\n");
    printf("Based on Golub & Van Loan 'Matrix Computations' Chapter 1\n");
    printf("=================================================================\n");
    printf("Matrix dimensions: %d x %d\n", matrix_size, matrix_size);
    printf("Warmup runs: %d, Test runs: %d\n", warmup_runs, test_runs);
    printf("Total FLOPs per multiplication: %.0f\n", 2.0 * pow(matrix_size, 3));
    printf("\n");
    
    // Initialize random seed
    srand(42);  // Fixed seed for reproducible results
    
    // Create test matrices
    Matrix *A = create_matrix(matrix_size, matrix_size);
    Matrix *B = create_matrix(matrix_size, matrix_size);
    
    init_random_matrix(A);
    init_random_matrix(B);
    
    // Verify correctness first
    verify_correctness(A, B);
    
    // Benchmark all algorithms
    printf("Running benchmarks...\n\n");
    
    PerfResult results[10];
    int result_count = 0;
    
    // Basic algorithms
    const char* alg_names[] = {
        "ijk (dot product)", "jik", "saxpy", "outer product", 
        "ikj (modular)", "kij", "ikj (inlined)"
    };
    MatMulFunc alg_funcs[] = {
        matmul_ijk, matmul_jik, matmul_saxpy, matmul_outer_product, 
        matmul_ikj, matmul_kij, matmul_ikj_inlined
    };
    
    for (int i = 0; i < 7; i++) {
        results[result_count++] = benchmark_algorithm(alg_funcs[i], alg_names[i], 
                                                    A, B, warmup_runs, test_runs);
    }
    
    // Test different block sizes
    int block_sizes[] = {32, 64, 128};
    for (int i = 0; i < 3; i++) {
        if (block_sizes[i] <= matrix_size) {
            results[result_count++] = benchmark_blocked(A, B, block_sizes[i], 
                                                      warmup_runs, test_runs);
        }
    }
    
    // Display results
    print_performance_results(results, result_count);
    
    // Educational notes
    printf("EDUCATIONAL NOTES:\n");
    printf("- Compare 'ikj (modular)' vs 'ikj (inlined)' to see function call overhead\n");
    printf("- Different loop orders show importance of memory access patterns\n");
    printf("- Blocked algorithms demonstrate cache optimization techniques\n");
    printf("- Each module corresponds to concepts from specific book sections\n");
    printf("\nModular structure makes it easy to experiment with variants!\n");
    printf("Usage: %s [matrix_size] [warmup_runs] [test_runs]\n", argv[0]);
    
    // Cleanup
    free_matrix(A);
    free_matrix(B);
    
    return 0;
}
