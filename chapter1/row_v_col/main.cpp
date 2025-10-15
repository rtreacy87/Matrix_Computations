#include <iostream>
#include "gaxpy.h"
#include "../src/benchmark.h"

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
    
    std::cout << "\nNote: Speedup < 1.0 means first implementation is faster.\n";
    std::cout << "      Speedup > 1.0 means second implementation is faster.\n";
    
    return 0;
}
