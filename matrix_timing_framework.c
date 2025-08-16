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

// Timer utilities
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

// Matrix utilities
Matrix* create_matrix(int rows, int cols) {
    Matrix *m = malloc(sizeof(Matrix));
    m->rows = rows;
    m->cols = cols;
    m->data = calloc(rows * cols, sizeof(double));
    return m;
}

void free_matrix(Matrix *m) {
    free(m->data);
    free(m);
}

// Access matrix element (row-major storage)
#define MAT(m, i, j) ((m)->data[(i) * (m)->cols + (j)])

void init_random_matrix(Matrix *m) {
    for (int i = 0; i < m->rows * m->cols; i++) {
        m->data[i] = (double)rand() / RAND_MAX * 2.0 - 1.0;
    }
}

void zero_matrix(Matrix *m) {
    memset(m->data, 0, m->rows * m->cols * sizeof(double));
}

void copy_matrix(Matrix *dest, Matrix *src) {
    memcpy(dest->data, src->data, src->rows * src->cols * sizeof(double));
}

// =============================================================================
// MATRIX MULTIPLICATION ALGORITHMS (from Golub & Van Loan Chapter 1)
// =============================================================================

// Algorithm 1.1.5 (ijk Matrix Multiplication) - Dot Product Version
void matmul_ijk(Matrix *C, Matrix *A, Matrix *B) {
    int m = A->rows, r = A->cols, n = B->cols;
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < r; k++) {
                MAT(C, i, j) += MAT(A, i, k) * MAT(B, k, j);
            }
        }
    }
}

// jik variant - column-oriented access
void matmul_jik(Matrix *C, Matrix *A, Matrix *B) {
    int m = A->rows, r = A->cols, n = B->cols;
    
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            for (int k = 0; k < r; k++) {
                MAT(C, i, j) += MAT(A, i, k) * MAT(B, k, j);
            }
        }
    }
}

// Algorithm 1.1.7 (Saxpy Matrix Multiplication)
void matmul_saxpy(Matrix *C, Matrix *A, Matrix *B) {
    int m = A->rows, r = A->cols, n = B->cols;
    
    for (int j = 0; j < n; j++) {
        for (int k = 0; k < r; k++) {
            // C(:,j) = C(:,j) + A(:,k) * B(k,j)
            double b_kj = MAT(B, k, j);
            for (int i = 0; i < m; i++) {
                MAT(C, i, j) += MAT(A, i, k) * b_kj;
            }
        }
    }
}

// Algorithm 1.1.8 (Outer Product Matrix Multiplication)
void matmul_outer_product(Matrix *C, Matrix *A, Matrix *B) {
    int m = A->rows, r = A->cols, n = B->cols;
    
    for (int k = 0; k < r; k++) {
        // C = C + A(:,k) * B(k,:)
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                MAT(C, i, j) += MAT(A, i, k) * MAT(B, k, j);
            }
        }
    }
}

// ikj variant - good for cache
void matmul_ikj(Matrix *C, Matrix *A, Matrix *B) {
    int m = A->rows, r = A->cols, n = B->cols;
    
    for (int i = 0; i < m; i++) {
        for (int k = 0; k < r; k++) {
            double a_ik = MAT(A, i, k);
            for (int j = 0; j < n; j++) {
                MAT(C, i, j) += a_ik * MAT(B, k, j);
            }
        }
    }
}

// kij variant - outer product style
void matmul_kij(Matrix *C, Matrix *A, Matrix *B) {
    int m = A->rows, r = A->cols, n = B->cols;
    
    for (int k = 0; k < r; k++) {
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                MAT(C, i, j) += MAT(A, i, k) * MAT(B, k, j);
            }
        }
    }
}

// =============================================================================
// LEVEL-1 and LEVEL-2 BLAS OPERATIONS
// =============================================================================

// Dot product: alpha = x^T * y
double dot_product(double *x, double *y, int n) {
    double result = 0.0;
    for (int i = 0; i < n; i++) {
        result += x[i] * y[i];
    }
    return result;
}

// SAXPY: y = y + a*x
void saxpy(double *y, double alpha, double *x, int n) {
    for (int i = 0; i < n; i++) {
        y[i] += alpha * x[i];
    }
}

// Matrix-vector multiplication: y = y + A*x
void matrix_vector_mult(Matrix *A, double *x, double *y) {
    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->cols; j++) {
            y[i] += MAT(A, i, j) * x[j];
        }
    }
}

// =============================================================================
// PERFORMANCE MEASUREMENT FRAMEWORK
// =============================================================================

typedef void (*MatMulFunc)(Matrix*, Matrix*, Matrix*);

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

void print_performance_results(PerfResult *results, int num_results) {
    printf("\n");
    printf("=================================================================\n");
    printf("PERFORMANCE RESULTS\n");
    printf("=================================================================\n");
    printf("%-20s %12s %12s %12s\n", "Algorithm", "Time (s)", "MFLOPS", "Relative");
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
        printf("%-20s %12.6f %12.2f %12.2fx\n", 
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

void verify_correctness(Matrix *A, Matrix *B) {
    printf("Verifying algorithm correctness...\n");
    
    Matrix *C1 = create_matrix(A->rows, B->cols);
    Matrix *C2 = create_matrix(A->rows, B->cols);
    
    // Reference: ijk method
    matmul_ijk(C1, A, B);
    
    // Test other methods
    const char* names[] = {"jik", "saxpy", "outer_product", "ikj", "kij"};
    MatMulFunc funcs[] = {matmul_jik, matmul_saxpy, matmul_outer_product, 
                         matmul_ikj, matmul_kij};
    
    for (int test = 0; test < 5; test++) {
        zero_matrix(C2);
        funcs[test](C2, A, B);
        
        // Check if results match
        double max_diff = 0.0;
        for (int i = 0; i < C1->rows * C1->cols; i++) {
            double diff = fabs(C1->data[i] - C2->data[i]);
            if (diff > max_diff) max_diff = diff;
        }
        
        if (max_diff < 1e-12) {
            printf("  %s: PASS\n", names[test]);
        } else {
            printf("  %s: FAIL (max diff = %e)\n", names[test], max_diff);
        }
    }
    
    free_matrix(C1);
    free_matrix(C2);
    printf("\n");
}

// =============================================================================
// MAIN BENCHMARK PROGRAM
// =============================================================================

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
    printf("MATRIX MULTIPLICATION BENCHMARK\n");
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
    
    PerfResult results[6];
    const char* alg_names[] = {"ijk (dot product)", "jik", "saxpy", 
                              "outer product", "ikj (cache-friendly)", "kij"};
    MatMulFunc alg_funcs[] = {matmul_ijk, matmul_jik, matmul_saxpy, 
                             matmul_outer_product, matmul_ikj, matmul_kij};
    
    for (int i = 0; i < 6; i++) {
        results[i] = benchmark_algorithm(alg_funcs[i], alg_names[i], 
                                       A, B, warmup_runs, test_runs);
    }
    
    // Display results
    print_performance_results(results, 6);
    
    // Educational notes
    printf("EDUCATIONAL NOTES:\n");
    printf("- Different loop orders access memory in different patterns\n");
    printf("- 'ikj' is often fastest due to better cache locality\n");
    printf("- 'saxpy' version corresponds to Algorithm 1.1.7 in the book\n");
    printf("- 'outer product' corresponds to Algorithm 1.1.8\n");
    printf("- Performance differences demonstrate importance of memory hierarchy\n");
    printf("\nTry different matrix sizes to see how performance scales!\n");
    printf("Usage: %s [matrix_size] [warmup_runs] [test_runs]\n", argv[0]);
    
    // Cleanup
    free_matrix(A);
    free_matrix(B);
    
    return 0;
}