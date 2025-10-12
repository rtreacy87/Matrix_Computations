#include <iostream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <string>
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

// Compare two gaxpy implementations across multiple matrix sizes
void compare_implementations(
    void (*gaxpy_func1)(const Matrix&, const std::vector<double>&, std::vector<double>&),
    void (*gaxpy_func2)(const Matrix&, const std::vector<double>&, std::vector<double>&),
    const std::string& name1,
    const std::string& name2,
    const std::vector<std::pair<int, int>>& sizes,
    int iterations = 100) {
    
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Performance Comparison: " << name1 << " vs " << name2 << "\n";
    std::cout << "====================================================\n\n";
    
    for (auto [m, n] : sizes) {
        std::cout << "Matrix size: " << m << " x " << n << "\n";
        std::cout << "Iterations: " << iterations << "\n";
        
        // Initialize data
        Matrix A(m, n);
        A.fill_random();
        
        std::vector<double> x(n);
        
        // Fill x with random values
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-1.0, 1.0);
        for (auto& val : x) {
            val = dis(gen);
        }
        
        // Create shared configuration
        BenchmarkConfig config = {A, x, iterations};
        
        // Each implementation gets its own output vector
        std::vector<double> y1(m, 0.0);
        std::vector<double> y2(m, 0.0);
        
        // Benchmark first implementation
        double time1 = benchmark_gaxpy(gaxpy_func1, config, y1);
        
        // Benchmark second implementation
        double time2 = benchmark_gaxpy(gaxpy_func2, config, y2);
        
        // Calculate speedup (time2 / time1)
        double speedup = time2 / time1;
        
        std::cout << "  " << std::left << std::setw(20) << (name1 + ":") 
                  << std::right << std::setw(10) << time1 << " ms\n";
        std::cout << "  " << std::left << std::setw(20) << (name2 + ":") 
                  << std::right << std::setw(10) << time2 << " ms\n";
        std::cout << "  Speedup (" << name1 << "/" << name2 << "): " 
                  << std::setw(8) << speedup << "x\n";
        
        // Verify results match (within floating-point tolerance)
        double max_diff = 0.0;
        for (int i = 0; i < m; i++) {
            max_diff = std::max(max_diff, std::abs(y1[i] - y2[i]));
        }
        std::cout << "  Max difference:  " << std::setw(10) << max_diff;
        
        if (max_diff > 1e-10) {
            std::cout << " ⚠️  WARNING: Results differ!";
        } else {
            std::cout << " ✓";
        }
        std::cout << "\n\n";
    }
}

int main() {
    // Define test sizes
    std::vector<std::pair<int, int>> sizes = {
        {100, 100},
        {500, 500},
        {1000, 1000},
        {2000, 2000},
        {5000, 5000}
    };
    
    // Compare row-oriented vs column-oriented
    compare_implementations(
        gaxpy_row_oriented,
        gaxpy_column_oriented,
        "Row-oriented",
        "Column-oriented",
        sizes,
        100  // iterations
    );
    
    // Example: Add more comparisons easily!
    // compare_implementations(
    //     gaxpy_row_oriented,
    //     gaxpy_blocked,
    //     "Row-oriented",
    //     "Blocked",
    //     sizes,
    //     100
    // );
    
    std::cout << "\nNote: Speedup < 1.0 means " << "first implementation" << " is faster.\n";
    std::cout << "      Speedup > 1.0 means " << "second implementation" << " is faster.\n";
    
    return 0;
}
