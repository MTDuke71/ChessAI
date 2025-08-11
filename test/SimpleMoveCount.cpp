#include <iostream>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h"

int main() {
    std::cout << "Simple Move Count Comparison\n";
    std::cout << "============================\n\n";
    
    Board board;
    FastMoveGenerator fastGen;
    MoveGenerator originalGen;
    
    // Test starting position
    FastMoveGenerator::MoveList fastMoves;
    fastGen.generateLegalMoves(board, true, fastMoves);
    
    std::vector<uint16_t> originalMoves = originalGen.generateLegalMoves(board, true);
    
    std::cout << "Starting position:\n";
    std::cout << "  FastMoveGenerator: " << fastMoves.count << " moves\n";
    std::cout << "  Original:          " << originalMoves.size() << " moves\n";
    
    if (fastMoves.count != originalMoves.size()) {
        std::cout << "  ❌ MOVE COUNT MISMATCH!\n";
    } else {
        std::cout << "  ✅ Move counts match\n";
    }
    
    return 0;
}
