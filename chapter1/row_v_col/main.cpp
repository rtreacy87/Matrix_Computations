#include <iostream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include "gaxpy.h"

// Simple timer class using high-resolution clock
class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    
public:
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    double elapsed_ms() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return duration.count() / 1000.0;  // Convert to milliseconds
    }
};

// Test harness - measures performance of a gaxpy implementation
double benchmark_gaxpy(void (*gaxpy_func)(const Matrix&, const std::vector<double>&, std::vector<double>&),
                       const Matrix& A, 
                       const std::vector<double>& x, 
                       std::vector<double>& y,
                       int iterations) {
    Timer timer;
    
    // Warm-up run
    gaxpy_func(A, x, y);
    
    // Reset y
    std::fill(y.begin(), y.end(), 0.0);
    
    // Timed runs
    timer.start();
    for (int iter = 0; iter < iterations; iter++) {
        gaxpy_func(A, x, y);
    }
    double total_time = timer.elapsed_ms();
    
    return total_time / iterations;  // Average time per iteration
}

int main() {
    // Test different matrix sizes
    std::vector<std::pair<int, int>> sizes = {
        {100, 100},
        {500, 500},
        {1000, 1000},
        {2000, 2000},
        {5000, 5000}
    };
    
    int iterations = 100;  // Number of iterations for averaging
    
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Gaxpy Performance Comparison (C++ Row-Major Storage)\n";
    std::cout << "====================================================\n\n";
    
    for (auto [m, n] : sizes) {
        std::cout << "Matrix size: " << m << " x " << n << "\n";
        std::cout << "Iterations: " << iterations << "\n";
        
        // Initialize data
        Matrix A(m, n);
        A.fill_random();
        
        std::vector<double> x(n);
        std::vector<double> y_row(m, 0.0);
        std::vector<double> y_col(m, 0.0);
        
        // Fill x with random values
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-1.0, 1.0);
        for (auto& val : x) {
            val = dis(gen);
        }
        
        // Benchmark row-oriented
        double time_row = benchmark_gaxpy(gaxpy_row_oriented, A, x, y_row, iterations);
        
        // Benchmark column-oriented
        double time_col = benchmark_gaxpy(gaxpy_column_oriented, A, x, y_col, iterations);
        
        // Calculate speedup
        double speedup = time_col / time_row;
        
        std::cout << "  Row-oriented:    " << std::setw(10) << time_row << " ms\n";
        std::cout << "  Column-oriented: " << std::setw(10) << time_col << " ms\n";
        std::cout << "  Speedup (row/col): " << std::setw(8) << speedup << "x\n";
        
        // Verify results match (within floating-point tolerance)
        double max_diff = 0.0;
        for (int i = 0; i < m; i++) {
            max_diff = std::max(max_diff, std::abs(y_row[i] - y_col[i]));
        }
        std::cout << "  Max difference:  " << std::setw(10) << max_diff << "\n";
        std::cout << "\n";
        
        // Add benchmarks for your new implementations here!
        // Example:
        // std::vector<double> y_new(m, 0.0);
        // double time_new = benchmark_gaxpy(gaxpy_blocked, A, x, y_new, iterations);
        // std::cout << "  Blocked version: " << std::setw(10) << time_new << " ms\n";
    }
    
    std::cout << "Note: In C++, matrices are stored in row-major order,\n";
    std::cout << "so row-oriented access should be faster due to better cache locality.\n";
    
    return 0;
}
