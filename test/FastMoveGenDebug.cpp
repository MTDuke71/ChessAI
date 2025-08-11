#include <iostream>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h" // For comparison
#include "MoveEncoding.h" // For decodeMove

int main() {
    std::cout << "FastMoveGenerator Debug Test\n";
    std::cout << "============================\n\n";
    
    Board board;
    FastMoveGenerator fastGen;
    MoveGenerator originalGen;
    
    // Test move generation at depth 1
    FastMoveGenerator::MoveList fastMoves;
    fastGen.generateLegalMoves(board, true, fastMoves);
    
    auto originalMoves = originalGen.generateLegalMoves(board, true);
    
    std::cout << "Original MoveGenerator: " << originalMoves.size() << " moves\n";
    std::cout << "Fast MoveGenerator: " << fastMoves.count << " moves\n\n";
    
    std::cout << "Original moves:\n";
    for (const auto& move : originalMoves) {
        std::cout << "  " << decodeMove(move) << "\n";
    }
    
    std::cout << "\nFast moves:\n";
    for (int i = 0; i < fastMoves.count; ++i) {
        std::cout << "  " << fastMoves.moves[i].toAlgebraic() << "\n";
    }
    
    return 0;
}
