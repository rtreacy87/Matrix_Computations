// ===========================================================================
// verification.h - Correctness testing function declarations  
// ===========================================================================
#ifndef VERIFICATION_H
#define VERIFICATION_H

#include "matrix_types.h"

// Function declarations
double matrix_max_diff(Matrix *A, Matrix *B);
void verify_correctness(Matrix *A, Matrix *B);

#endif // VERIFICATION_H
