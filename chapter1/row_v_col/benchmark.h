#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <vector>
#include <string>
#include <chrono>
#include "gaxpy.h"

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
    void start();
    double elapsed_ms();
};

// Benchmark a single gaxpy implementation
// Returns average time per iteration in milliseconds
double benchmark_gaxpy(
    void (*gaxpy_func)(const Matrix&, const std::vector<double>&, std::vector<double>&),
    const BenchmarkConfig& config,
    std::vector<double>& y);

// Compare two gaxpy implementations across multiple matrix sizes
void compare_implementations(
    void (*gaxpy_func1)(const Matrix&, const std::vector<double>&, std::vector<double>&),
    void (*gaxpy_func2)(const Matrix&, const std::vector<double>&, std::vector<double>&),
    const std::string& name1,
    const std::string& name2,
    const std::vector<std::pair<int, int>>& sizes,
    int iterations = 100);

#endif // BENCHMARK_H
