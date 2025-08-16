// ===========================================================================
// verification.c - Correctness testing
// ===========================================================================
#include "matrix_types.h"
#include "matrix_utils.h"
#include "matmul_optimized.h"
#include "matmul_basic.h"

double matrix_max_diff(Matrix *A, Matrix *B) {
    double max_diff = 0.0;
    for (int i = 0; i < A->rows * A->cols; i++) {
        double diff = fabs(A->data[i] - B->data[i]);
        if (diff > max_diff) max_diff = diff;
    }
    return max_diff;
}

void verify_correctness(Matrix *A, Matrix *B) {
    printf("Verifying algorithm correctness...\n");
    
    Matrix *C_ref = create_matrix(A->rows, B->cols);
    Matrix *C_test = create_matrix(A->rows, B->cols);
    
    // Reference: ijk method
    matmul_ijk(C_ref, A, B);
    
    // Test other methods
    const char* names[] = {"jik", "saxpy", "outer_product", "ikj", "kij", "ikj_inlined"};
    MatMulFunc funcs[] = {matmul_jik, matmul_saxpy, matmul_outer_product, 
                         matmul_ikj, matmul_kij, matmul_ikj_inlined};
    
    for (int test = 0; test < 6; test++) {
        zero_matrix(C_test);
        funcs[test](C_test, A, B);
        
        double max_diff = matrix_max_diff(C_ref, C_test);
        
        if (max_diff < 1e-12) {
            printf("  %s: PASS\n", names[test]);
        } else {
            printf("  %s: FAIL (max diff = %e)\n", names[test], max_diff);
        }
    }
    
    // Test blocked version
    zero_matrix(C_test);
    matmul_blocked(C_test, A, B, 64);
    double max_diff = matrix_max_diff(C_ref, C_test);
    if (max_diff < 1e-12) {
        printf("  blocked: PASS\n");
    } else {
        printf("  blocked: FAIL (max diff = %e)\n", max_diff);
    }
    
    free_matrix(C_ref);
    free_matrix(C_test);
    printf("\n");
}
