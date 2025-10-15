#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include <vector>
#include <random>
#include <chrono>

// Matrix class with row-major storage (C++ default)
class Matrix {
public:
    int m, n;
    std::vector<double> data;
    
    Matrix(int rows, int cols) : m(rows), n(cols), data(rows * cols) {}
    
    // Access element at (i, j)
    double& operator()(int i, int j) {
        return data[i * n + j];  // Row-major indexing
    }
    
    // Access element at (i, j) - const version
    const double& operator()(int i, int j) const {
        return data[i * n + j];
    }
    
    // Fill matrix with random values between -1.0 and 1.0
    void fill_random() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-1.0, 1.0);
        for (auto& val : data) {
            val = dis(gen);
        }
    }
};

// Benchmark configuration - groups shared test parameters
struct BenchmarkConfig {
    const Matrix& A;
    const std::vector<double>& x;
    int iterations;
};

// Timer class for accurate performance measurements
class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    
public:
    // Start timing
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    // Get elapsed time in milliseconds
    double elapsed_ms() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return duration.count() / 1000.0;  // Convert to milliseconds
    }
};

#endif // MATRIX_UTILS_H
