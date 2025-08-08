#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include <iostream>

int main() {
    std::cout << "=== SIMPLE DEPTH 6 PERFT TEST ===\n";
    
    Board board;
    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    MoveGenerator gen;
    
    std::cout << "Computing depth 6 perft...\n";
    uint64_t result = perft(board, gen, 6);
    
    std::cout << "Result: " << result << "\n";
    std::cout << "Expected: 119060324\n";
    std::cout << "Difference: " << (int64_t)(result - 119060324) << "\n";
    
    if (result == 119060324) {
        std::cout << "✓ PERFECT MATCH!\n";
    } else {
        std::cout << "✗ MISMATCH DETECTED\n";
    }
    
    return 0;
}
