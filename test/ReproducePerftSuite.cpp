#include "../src/Perft.h"
#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include <iostream>

int main() {
    std::cout << "=== REPRODUCING PERFTSUITE APPROACH ===\n";
    
    // Mimic exactly what PerftSuiteTest does
    Board board;
    MoveGenerator generator;
    
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    std::cout << "Loading FEN: " << fen << "\n";
    if (!board.loadFEN(fen)) {
        std::cerr << "Failed to load FEN\n";
        return 1;
    }
    
    std::cout << "Computing depth 6 perft (PerftSuite style)...\n";
    uint64_t nodes = perft(board, generator, 6);
    
    std::cout << "Result: " << nodes << "\n";
    std::cout << "Expected: 119060324\n";
    std::cout << "Difference: " << (int64_t)(nodes - 119060324) << "\n";
    
    if (nodes == 119060324) {
        std::cout << "✓ MATCHES EXPECTED\n";
    } else {
        std::cout << "✗ MISMATCH DETECTED (-726 nodes)\n";
    }
    
    return 0;
}
