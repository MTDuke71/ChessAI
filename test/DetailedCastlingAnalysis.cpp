#include <iostream>
#include <set>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h"

int main() {
    std::cout << "Detailed Castling Analysis\n";
    std::cout << "==========================\n\n";
    
    Board board;
    board.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    
    FastMoveGenerator fastGen;
    MoveGenerator originalGen;
    
    FastMoveGenerator::MoveList fastMoves;
    fastGen.generateLegalMoves(board, true, fastMoves);
    
    std::vector<uint16_t> originalMoves = originalGen.generateLegalMoves(board, true);
    
    std::cout << "Position: " << board.getFEN() << "\n";
    std::cout << "FastMoveGenerator: " << fastMoves.count << " moves\n";
    std::cout << "Original: " << originalMoves.size() << " moves\n\n";
    
    // Convert to sets for comparison
    std::set<std::string> fastSet, originalSet;
    
    std::cout << "FastMoveGenerator moves:\n";
    for (int i = 0; i < fastMoves.count; ++i) {
        std::string move = fastMoves.moves[i].toAlgebraic();
        fastSet.insert(move);
        std::cout << "  " << (i+1) << ". " << move << "\n";
    }
    
    std::cout << "\nOriginal moves:\n";
    for (size_t i = 0; i < originalMoves.size(); ++i) {
        std::string move = indexToAlgebraic(originalMoves[i]);
        originalSet.insert(move);
        std::cout << "  " << (i+1) << ". " << move << "\n";
    }
    
    std::cout << "\nExtra moves in FastMoveGenerator:\n";
    for (const auto& move : fastSet) {
        if (originalSet.find(move) == originalSet.end()) {
            std::cout << "  EXTRA: " << move << "\n";
        }
    }
    
    std::cout << "\nMissing moves in FastMoveGenerator:\n";
    for (const auto& move : originalSet) {
        if (fastSet.find(move) == fastSet.end()) {
            std::cout << "  MISSING: " << move << "\n";
        }
    }
    
    return 0;
}
