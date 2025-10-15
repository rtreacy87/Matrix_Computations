#include <iostream>
#include "gaxpy.h"
#include "../src/benchmark.h"

int main() {
    std::vector<std::pair<int, int>> sizes = {
        {100, 100},
        {500, 500},
        {1000, 1000},
        {2000, 2000},
        {5000, 5000}
    };
    
    std::cout << "================================================================\n";
    std::cout << "Clean Code vs Performance: Does Modularization Cost Speed?\n";
    std::cout << "================================================================\n\n";
    
    // Test 1: Baseline vs Helper Function
    std::cout << "TEST 1: Simple Function Extraction\n";
    std::cout << "-----------------------------------\n";
    compare_implementations(
        gaxpy_nested_for_loop,
        gaxpy_modular,
        "Nested Loop",
        "Helper Function",
        sizes,
        100
    );
    
    std::cout << "\n";
    
    // Test 2: Baseline vs Lambda/Functional
    std::cout << "TEST 2: Modern C++ Lambda Style\n";
    std::cout << "--------------------------------\n";
    compare_implementations(
        gaxpy_nested_for_loop,
        gaxpy_functional,
        "Nested Loop",
        "Lambda Functional",
        sizes,
        100
    );
    
    std::cout << "\n";
    
    // Test 3: Baseline vs Explicit Inline Hint
    std::cout << "TEST 3: Explicit Inline Hint\n";
    std::cout << "-----------------------------\n";
    compare_implementations(
        gaxpy_nested_for_loop,
        gaxpy_inline_hint,
        "Nested Loop",
        "Explicit Inline",
        sizes,
        100
    );
    
    std::cout << "\n================================================================\n";
    std::cout << "INTERPRETATION GUIDE:\n";
    std::cout << "================================================================\n";
    std::cout << "Speedup ≈ 1.0 (0.95-1.05):\n";
    std::cout << "  ✓ Compiler successfully inlined the abstraction\n";
    std::cout << "  ✓ Clean code costs nothing with -O3 optimization\n";
    std::cout << "  ✓ You can write readable code without penalty!\n\n";
    
    std::cout << "Speedup > 1.2:\n";
    std::cout << "  ⚠ Function call overhead is measurable\n";
    std::cout << "  ⚠ Abstraction has a cost (but may still be worth it)\n";
    std::cout << "  ⚠ Consider: Does readability justify 20% slowdown?\n\n";
    
    std::cout << "Speedup < 0.95:\n";
    std::cout << "  ! Modular version is actually FASTER\n";
    std::cout << "  ! Compiler may have found better optimization path\n";
    std::cout << "  ! Sometimes abstraction helps compiler optimize!\n\n";
    
    std::cout << "KEY LESSON: With modern compilers at -O3, clean code abstractions\n";
    std::cout << "            usually compile to identical machine code. Write clear,\n";
    std::cout << "            maintainable code first, optimize only when profiling\n";
    std::cout << "            proves it's actually a bottleneck.\n";
    std::cout << "================================================================\n";
    
    return 0;
}
