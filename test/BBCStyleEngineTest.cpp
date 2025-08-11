#include "BBCStyleEngine.h"
#include "Board.h"
#include "MoveGenerator.h"
#include "Perft.h"
#include <chrono>
#include <iostream>

int main() {
    std::cout << "=== BBC-Style Engine Performance Test ===\n\n";
    
    // Test 1: Current Aphelion approach
    std::cout << "1. Current Aphelion Approach (depth 3):\n";
    Board board;
    MoveGenerator gen;
    
    auto start1 = std::chrono::steady_clock::now();
    double ms1;
    uint64_t nodes1 = perft(board, gen, 3, ms1);
    auto end1 = std::chrono::steady_clock::now();
    ms1 = std::chrono::duration<double, std::milli>(end1 - start1).count();
    
    std::cout << "   Nodes: " << nodes1 << "\n";
    std::cout << "   Time: " << ms1 << " ms\n";
    std::cout << "   NPS: " << (nodes1 / ms1 * 1000.0) << "\n\n";
    
    // Test 2: BBC-style direct approach  
    std::cout << "2. BBC-Style Direct Approach (basic move gen):\n";
    BBCStyleEngine bbcEngine;
    
    auto start2 = std::chrono::steady_clock::now();
    
    // Test basic operations
    uint64_t operations = 0;
    for (int i = 0; i < 1000000; i++) {
        bbcEngine.copyBoard();
        bbcEngine.updateOccupancies();
        bbcEngine.takeBack();
        operations += 3;
    }
    
    auto end2 = std::chrono::steady_clock::now();
    double ms2 = std::chrono::duration<double, std::milli>(end2 - start2).count();
    
    std::cout << "   Operations: " << operations << "\n";
    std::cout << "   Time: " << ms2 << " ms\n";
    std::cout << "   OPS: " << (operations / ms2 * 1000.0) << "\n\n";
    
    // Test 3: BBC-style bit manipulation speed
    std::cout << "3. BBC-Style Bit Manipulation Speed:\n";
    auto start3 = std::chrono::steady_clock::now();
    
    uint64_t bitOps = 0;
    uint64_t testBitboard = 0x00FF00FF00FF00FFULL;
    
    for (int i = 0; i < 10000000; i++) {
        for (int square = 0; square < 64; square++) {
            if (get_bit(testBitboard, square)) {
                pop_bit(testBitboard, square);
                set_bit(testBitboard, square);
                bitOps += 3;
            }
        }
    }
    
    auto end3 = std::chrono::steady_clock::now();
    double ms3 = std::chrono::duration<double, std::milli>(end3 - start3).count();
    
    std::cout << "   Bit operations: " << bitOps << "\n";
    std::cout << "   Time: " << ms3 << " ms\n";
    std::cout << "   Bit OPS: " << (bitOps / ms3 * 1000.0) << "\n\n";
    
    // Test 4: Attack checking comparison
    std::cout << "4. Attack Checking Speed Comparison:\n";
    auto start4 = std::chrono::steady_clock::now();
    
    uint64_t attacks = 0;
    for (int i = 0; i < 100000; i++) {
        for (int square = 0; square < 64; square++) {
            if (bbcEngine.isSquareAttacked(square, white)) attacks++;
            if (bbcEngine.isSquareAttacked(square, black)) attacks++;
        }
    }
    
    auto end4 = std::chrono::steady_clock::now();
    double ms4 = std::chrono::duration<double, std::milli>(end4 - start4).count();
    
    std::cout << "   Attack checks: " << (100000 * 64 * 2) << "\n";
    std::cout << "   Attacks found: " << attacks << "\n";  
    std::cout << "   Time: " << ms4 << " ms\n";
    std::cout << "   Attack checks/sec: " << (100000 * 64 * 2 / ms4 * 1000.0) << "\n\n";
    
    std::cout << "=== Analysis ===\n";
    std::cout << "BBC-style approach shows the potential for direct bitboard manipulation.\n";
    std::cout << "Key advantages:\n";
    std::cout << "- Direct memory access patterns\n";
    std::cout << "- Minimal abstraction overhead\n";  
    std::cout << "- Ultra-fast make/unmake via memcpy\n";
    std::cout << "- Streamlined attack checking\n";
    
    return 0;
}
