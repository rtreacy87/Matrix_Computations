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
                       const BenchmarkConfig& config, 
                       std::vector<double>& y) {
    Timer timer;
    
    // Warm-up run
    gaxpy_func(config.A, config.x, y);
    
    // Reset y
    std::fill(y.begin(), y.end(), 0.0);
    
    // Timed runs
    timer.start();
    for (int iter = 0; iter < config.iterations; iter++) {
        gaxpy_func(config.A, config.x, y);
    }
    double total_time = timer.elapsed_ms();
    
    return total_time / config.iterations;  // Average time per iteration
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
    std::cout << "Performance Comparison\n";
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

        BenchmarkConfig config = {A, x, iterations};

        // Benchmark row-oriented
        double time_row = benchmark_gaxpy(gaxpy_row_oriented, config, y_row);
        
        // Benchmark column-oriented
        double time_col = benchmark_gaxpy(gaxpy_column_oriented, config, y_col);
        
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
    
    return 0;
}
