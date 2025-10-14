#ifndef GAXPY_H
#define GAXPY_H

#include <vector>
#include <random>

// Matrix class with row-major storage (C++ default)
class Matrix {
public:
    int m, n;
    std::vector<double> data;
    
    Matrix(int rows, int cols) : m(rows), n(cols), data(rows * cols) {}
    
    double& operator()(int i, int j) {
        return data[i * n + j];  // Row-major indexing
    }
    
    const double& operator()(int i, int j) const {
        return data[i * n + j];
    }
    
    void fill_random() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-1.0, 1.0);
        for (auto& val : data) {
            val = dis(gen);
        }
    }
};



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
