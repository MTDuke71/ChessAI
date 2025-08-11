#include <iostream>
#include <chrono>
#include "Board.h"
#include "MoveGenerator.h"
#include "Perft.h"

int main() {
    std::cout << "Aphelion Engine - Perft 6 Test\n";
    std::cout << "===============================\n\n";
    
    // Create board in starting position
    Board board;
    MoveGenerator generator;
    
    std::cout << "Testing Perft on starting position:\n";
    
    // Test depth 6
    const int depth = 6;
    double ms = 0.0;
    
    std::cout << "\nTesting depth " << depth << ":\n";
    
    auto start = std::chrono::steady_clock::now();
    uint64_t nodes = perftDivide(board, generator, depth);
    auto end = std::chrono::steady_clock::now();
    
    ms = std::chrono::duration<double, std::milli>(end - start).count();
    
    std::cout << "\nDepth: " << depth << std::endl;
    std::cout << "Nodes: " << nodes << std::endl;
    std::cout << "Time: " << ms << " ms" << std::endl;
    
    if (ms > 0) {
        std::cout << "NPS: " << (long)(nodes / (ms / 1000.0)) << " nodes/second" << std::endl;
    }
    
    std::cout << "\nAphelion Performance Test Complete.\n";
    
    return 0;
}
