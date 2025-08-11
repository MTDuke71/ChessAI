#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h"
#include "MoveEncoding.h"
#include "IncrementalBoard.h"
#include <chrono>
#include <iostream>

// Test our BBC-style direct approach vs current string-based approach
int main() {
    std::cout << "=== BBC-Style Performance Comparison ===\n\n";
    
    Board board;
    MoveGenerator gen;
    FastMoveGenerator fastGen;
    
    // Test 1: Current approach (string conversions)
    std::cout << "1. Current String-Based Approach:\n";
    auto start1 = std::chrono::steady_clock::now();
    
    auto moves = gen.generateAllMoves(board, true);
    uint64_t stringMoves = 0;
    
    for (int i = 0; i < 100000; i++) { // 100K iterations (reduced for faster testing)
        for (auto move : moves) {
            std::string algebraic = decodeMove(move);
            uint16_t encoded = encodeMove(algebraic);
            stringMoves++;
        }
    }
    
    auto end1 = std::chrono::steady_clock::now();
    double ms1 = std::chrono::duration<double, std::milli>(end1 - start1).count();
    
    std::cout << "   String conversions: " << stringMoves << " in " << ms1 << " ms\n";
    std::cout << "   Rate: " << (stringMoves / ms1 * 1000.0) << " conversions/sec\n\n";
    
    // Test 2: BBC-style direct approach
    std::cout << "2. BBC-Style Direct Approach:\n";
    auto start2 = std::chrono::steady_clock::now();
    
    FastMoveGenerator::MoveList fastMoves;
    fastGen.generateLegalMoves(board, true, fastMoves);
    uint64_t directMoves = 0;
    
    for (int i = 0; i < 100000; i++) { // 100K iterations 
        for (int j = 0; j < fastMoves.count; j++) {
            const auto& move = fastMoves.moves[j];
            // Direct access - no conversions
            int from = move.from();
            int to = move.to();
            directMoves++;
        }
    }
    
    auto end2 = std::chrono::steady_clock::now();
    double ms2 = std::chrono::duration<double, std::milli>(end2 - start2).count();
    
    std::cout << "   Direct access: " << directMoves << " in " << ms2 << " ms\n";
    std::cout << "   Rate: " << (directMoves / ms2 * 1000.0) << " accesses/sec\n\n";
    
    std::cout << "Performance Ratio: " << (ms1 / ms2) << "x faster with direct approach\n";
    
    return 0;
}
