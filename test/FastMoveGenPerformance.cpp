#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include <iostream>
#include <chrono>

int main() {
    std::cout << "FastMoveGenerator Performance Test\n";
    std::cout << "==================================\n\n";
    
    Board board;
    MoveGenerator generator;
    
    std::cout << "Testing depth 6 perft with FASTMOVEGENERATOR:\n";
    
    auto start = std::chrono::steady_clock::now();
    uint64_t nodes = perft(board, generator, 6);
    auto end = std::chrono::steady_clock::now();
    
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    double nps = (nodes * 1000.0) / ms;
    
    std::cout << "Depth: 6\n";
    std::cout << "Nodes: " << nodes << "\n";
    std::cout << "Time: " << ms << " ms\n";
    std::cout << "NPS: " << static_cast<uint64_t>(nps) << " nodes/second\n\n";
    
    std::cout << "Expected nodes: 119,060,324\n";
    if (nodes == 119060324) {
        std::cout << "✓ CORRECT node count\n";
    } else {
        std::cout << "✗ INCORRECT node count\n";
    }
    
    return 0;
}
