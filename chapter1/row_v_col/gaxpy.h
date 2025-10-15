#ifndef GAXPY_H
#define GAXPY_H

#include <vector>
#include <random>
#include "matrix_utils.h"



// Function declarations for different gaxpy implementations
// y = y + A*x

void gaxpy_row_oriented(const Matrix& A, 
                       const std::vector<double>& x, 
                       std::vector<double>& y);

void gaxpy_column_oriented(const Matrix& A, 
                          const std::vector<double>& x, 
                          std::vector<double>& y);

// Add declarations for your new implementations here
// void gaxpy_blocked(const Matrix& A, const std::vector<double>& x, std::vector<double>& y);
// void gaxpy_optimized(const Matrix& A, const std::vector<double>& x, std::vector<double>& y);

#endif // GAXPY_H
