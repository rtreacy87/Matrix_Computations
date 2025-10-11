#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include "gaxpy.h"

// Simple test framework
class TestSuite {
private:
    int passed = 0;
    int failed = 0;
    std::string current_test;

public:
    void start_test(const std::string& name) {
        current_test = name;
    }

    void assert_true(bool condition, const std::string& message) {
        if (condition) {
            passed++;
            std::cout << "  ✓ " << message << "\n";
        } else {
            failed++;
            std::cout << "  ✗ FAILED: " << message << "\n";
        }
    }

    void assert_near(double actual, double expected, double tolerance, const std::string& message) {
        if (std::abs(actual - expected) <= tolerance) {
            passed++;
            std::cout << "  ✓ " << message << "\n";
        } else {
            failed++;
            std::cout << "  ✗ FAILED: " << message 
                      << " (expected " << expected 
                      << ", got " << actual << ")\n";
        }
    }

    void assert_vectors_equal(const std::vector<double>& v1, 
                             const std::vector<double>& v2, 
                             double tolerance,
                             const std::string& message) {
        if (v1.size() != v2.size()) {
            failed++;
            std::cout << "  ✗ FAILED: " << message 
                      << " (size mismatch: " << v1.size() 
                      << " vs " << v2.size() << ")\n";
            return;
        }

        double max_diff = 0.0;
        for (size_t i = 0; i < v1.size(); i++) {
            max_diff = std::max(max_diff, std::abs(v1[i] - v2[i]));
        }

        if (max_diff <= tolerance) {
            passed++;
            std::cout << "  ✓ " << message << " (max diff: " << max_diff << ")\n";
        } else {
            failed++;
            std::cout << "  ✗ FAILED: " << message 
                      << " (max diff: " << max_diff 
                      << ", tolerance: " << tolerance << ")\n";
        }
    }

    void print_summary() {
        std::cout << "\n========================================\n";
        std::cout << "Test Summary\n";
        std::cout << "========================================\n";
        std::cout << "Passed: " << passed << "\n";
        std::cout << "Failed: " << failed << "\n";
        std::cout << "Total:  " << (passed + failed) << "\n";
        
        if (failed == 0) {
            std::cout << "\n✓ All tests passed!\n";
        } else {
            std::cout << "\n✗ Some tests failed!\n";
        }
    }

    bool all_passed() const { return failed == 0; }
};

// Test Matrix class basic functionality
void test_matrix_class(TestSuite& suite) {
    std::cout << "\n[Test: Matrix Class]\n";
    
    // Test construction
    Matrix A(3, 4);
    suite.assert_true(A.m == 3 && A.n == 4, "Matrix dimensions correct");
    suite.assert_true(A.data.size() == 12, "Matrix data size correct");
    
    // Test element access
    A(0, 0) = 1.0;
    A(1, 2) = 5.0;
    A(2, 3) = 9.0;
    suite.assert_near(A(0, 0), 1.0, 1e-10, "Element (0,0) access");
    suite.assert_near(A(1, 2), 5.0, 1e-10, "Element (1,2) access");
    suite.assert_near(A(2, 3), 9.0, 1e-10, "Element (2,3) access");
    
    // Test fill_random
    A.fill_random();
    bool has_nonzero = false;
    for (const auto& val : A.data) {
        if (val != 0.0) has_nonzero = true;
    }
    suite.assert_true(has_nonzero, "fill_random produces non-zero values");
}

// Test gaxpy with known values
void test_known_values(TestSuite& suite) {
    std::cout << "\n[Test: Known Values]\n";
    
    // Simple 2x2 test
    // A = [1 2]   x = [1]   y_initial = [0]
    //     [3 4]       [1]                [0]
    // Result: y = [3, 7]
    
    Matrix A(2, 2);
    A(0, 0) = 1; A(0, 1) = 2;
    A(1, 0) = 3; A(1, 1) = 4;
    
    std::vector<double> x = {1.0, 1.0};
    std::vector<double> y_expected = {3.0, 7.0};
    
    // Test row-oriented
    std::vector<double> y_row(2, 0.0);
    gaxpy_row_oriented(A, x, y_row);
    suite.assert_vectors_equal(y_row, y_expected, 1e-10, "Row-oriented: 2x2 known values");
    
    // Test column-oriented
    std::vector<double> y_col(2, 0.0);
    gaxpy_column_oriented(A, x, y_col);
    suite.assert_vectors_equal(y_col, y_expected, 1e-10, "Column-oriented: 2x2 known values");
}

// Test that different implementations produce same results
void test_implementation_equivalence(TestSuite& suite) {
    std::cout << "\n[Test: Implementation Equivalence]\n";
    
    // Test various matrix sizes
    std::vector<std::pair<int, int>> sizes = {
        {1, 1},      // Minimal
        {1, 10},     // Single row
        {10, 1},     // Single column
        {5, 5},      // Small square
        {10, 20},    // Rectangular
        {100, 100}   // Larger
    };
    
    for (auto [m, n] : sizes) {
        Matrix A(m, n);
        A.fill_random();
        
        std::vector<double> x(n);
        for (auto& val : x) {
            val = static_cast<double>(rand()) / RAND_MAX;
        }
        
        std::vector<double> y_row(m, 0.0);
        std::vector<double> y_col(m, 0.0);
        
        gaxpy_row_oriented(A, x, y_row);
        gaxpy_column_oriented(A, x, y_col);
        
        std::string msg = "Equivalence for " + std::to_string(m) + "x" + std::to_string(n);
        suite.assert_vectors_equal(y_row, y_col, 1e-10, msg);
    }
}

// Test edge cases
void test_edge_cases(TestSuite& suite) {
    std::cout << "\n[Test: Edge Cases]\n";
    
    // Test 1x1 matrix
    {
        Matrix A(1, 1);
        A(0, 0) = 5.0;
        std::vector<double> x = {2.0};
        std::vector<double> y_row(1, 0.0);
        std::vector<double> y_col(1, 0.0);
        
        gaxpy_row_oriented(A, x, y_row);
        gaxpy_column_oriented(A, x, y_col);
        
        suite.assert_near(y_row[0], 10.0, 1e-10, "1x1 matrix row-oriented");
        suite.assert_near(y_col[0], 10.0, 1e-10, "1x1 matrix column-oriented");
    }
    
    // Test with zero vector x
    {
        Matrix A(3, 3);
        A.fill_random();
        std::vector<double> x(3, 0.0);
        std::vector<double> y_row(3, 0.0);
        
        gaxpy_row_oriented(A, x, y_row);
        
        bool all_zero = true;
        for (auto val : y_row) {
            if (std::abs(val) > 1e-10) all_zero = false;
        }
        suite.assert_true(all_zero, "Zero input vector produces zero output");
    }
    
    // Test with zero matrix
    {
        Matrix A(3, 3);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                A(i, j) = 0.0;
            }
        }
        std::vector<double> x(3, 1.0);
        std::vector<double> y_col(3, 0.0);
        
        gaxpy_column_oriented(A, x, y_col);
        
        bool all_zero = true;
        for (auto val : y_col) {
            if (std::abs(val) > 1e-10) all_zero = false;
        }
        suite.assert_true(all_zero, "Zero matrix produces zero output");
    }
    
    // Test single row matrix
    {
        Matrix A(1, 5);
        for (int j = 0; j < 5; j++) {
            A(0, j) = j + 1.0;  // [1, 2, 3, 4, 5]
        }
        std::vector<double> x(5, 1.0);
        std::vector<double> y_row(1, 0.0);
        std::vector<double> y_col(1, 0.0);
        
        gaxpy_row_oriented(A, x, y_row);
        gaxpy_column_oriented(A, x, y_col);
        
        suite.assert_near(y_row[0], 15.0, 1e-10, "Single row matrix row-oriented");
        suite.assert_near(y_col[0], 15.0, 1e-10, "Single row matrix column-oriented");
    }
    
    // Test single column matrix
    {
        Matrix A(5, 1);
        for (int i = 0; i < 5; i++) {
            A(i, 0) = i + 1.0;  // [1; 2; 3; 4; 5]
        }
        std::vector<double> x = {2.0};
        std::vector<double> y_row(5, 0.0);
        std::vector<double> y_col(5, 0.0);
        
        gaxpy_row_oriented(A, x, y_row);
        gaxpy_column_oriented(A, x, y_col);
        
        std::vector<double> expected = {2.0, 4.0, 6.0, 8.0, 10.0};
        suite.assert_vectors_equal(y_row, expected, 1e-10, "Single column matrix row-oriented");
        suite.assert_vectors_equal(y_col, expected, 1e-10, "Single column matrix column-oriented");
    }
}

// Test with non-zero initial y
void test_accumulation(TestSuite& suite) {
    std::cout << "\n[Test: Accumulation (y += Ax)]\n";
    
    Matrix A(2, 2);
    A(0, 0) = 1; A(0, 1) = 2;
    A(1, 0) = 3; A(1, 1) = 4;
    
    std::vector<double> x = {1.0, 1.0};
    std::vector<double> y_row = {10.0, 20.0};  // Non-zero initial values
    std::vector<double> y_col = {10.0, 20.0};
    
    gaxpy_row_oriented(A, x, y_row);
    gaxpy_column_oriented(A, x, y_col);
    
    std::vector<double> expected = {13.0, 27.0};  // [10 + 3, 20 + 7]
    
    suite.assert_vectors_equal(y_row, expected, 1e-10, "Row-oriented accumulation");
    suite.assert_vectors_equal(y_col, expected, 1e-10, "Column-oriented accumulation");
}

// Test identity matrix
void test_identity_matrix(TestSuite& suite) {
    std::cout << "\n[Test: Identity Matrix]\n";
    
    const int n = 5;
    Matrix I(n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            I(i, j) = (i == j) ? 1.0 : 0.0;
        }
    }
    
    std::vector<double> x(n);
    for (int i = 0; i < n; i++) {
        x[i] = i + 1.0;  // [1, 2, 3, 4, 5]
    }
    
    std::vector<double> y_row(n, 0.0);
    std::vector<double> y_col(n, 0.0);
    
    gaxpy_row_oriented(I, x, y_row);
    gaxpy_column_oriented(I, x, y_col);
    
    suite.assert_vectors_equal(y_row, x, 1e-10, "Identity matrix row-oriented (I*x = x)");
    suite.assert_vectors_equal(y_col, x, 1e-10, "Identity matrix column-oriented (I*x = x)");
}

int main() {
    std::cout << "========================================\n";
    std::cout << "Gaxpy Implementation Test Suite\n";
    std::cout << "========================================\n";
    
    TestSuite suite;
    
    // Run all tests
    test_matrix_class(suite);
    test_known_values(suite);
    test_implementation_equivalence(suite);
    test_edge_cases(suite);
    test_accumulation(suite);
    test_identity_matrix(suite);
    
    // Print summary
    suite.print_summary();
    
    return suite.all_passed() ? 0 : 1;
}
