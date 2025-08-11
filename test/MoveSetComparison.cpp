#include <iostream>
#include <set>
#include <string>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h"

int main() {
    std::cout << "Move Generation Comparison - Starting Position\n";
    std::cout << "==============================================\n\n";
    
    Board board;
    FastMoveGenerator fastGen;
    MoveGenerator originalGen;
    
    // Generate moves with both generators
    FastMoveGenerator::MoveList fastMoves;
    fastGen.generateLegalMoves(board, true, fastMoves);
    
    std::vector<uint16_t> originalMoves = originalGen.generateLegalMoves(board, true);
    
    std::cout << "FastMoveGenerator: " << fastMoves.count << " moves\n";
    std::cout << "Original:          " << originalMoves.size() << " moves\n\n";
    
    // Convert fast moves to strings and check for duplicates
    std::set<std::string> fastMoveSet;
    std::vector<std::string> fastMoveStrings;
    
    for (int i = 0; i < fastMoves.count; ++i) {
        std::string moveStr = fastMoves.moves[i].toAlgebraic();
        fastMoveStrings.push_back(moveStr);
        
        if (fastMoveSet.find(moveStr) != fastMoveSet.end()) {
            std::cout << "DUPLICATE MOVE in FastMoveGenerator: " << moveStr << "\n";
        }
        fastMoveSet.insert(moveStr);
    }
    
    // Check for duplicates in original moves
    std::set<std::string> originalMoveSet;
    std::vector<std::string> originalMoveStrings;
    
    for (const auto& move : originalMoves) {
        std::string moveStr = indexToAlgebraic(move);
        originalMoveStrings.push_back(moveStr);
        
        if (originalMoveSet.find(moveStr) != originalMoveSet.end()) {
            std::cout << "DUPLICATE MOVE in Original: " << moveStr << "\n";
        }
        originalMoveSet.insert(moveStr);
    }
    
    std::cout << "\nFastMoveGenerator unique moves: " << fastMoveSet.size() << "\n";
    std::cout << "Original unique moves:          " << originalMoveSet.size() << "\n\n";
    
    // Find moves in FastMoveGenerator but not in Original
    std::cout << "Moves in FastMoveGenerator but NOT in Original:\n";
    for (const auto& move : fastMoveSet) {
        if (originalMoveSet.find(move) == originalMoveSet.end()) {
            std::cout << "  " << move << "\n";
        }
    }
    
    // Find moves in Original but not in FastMoveGenerator
    std::cout << "\nMoves in Original but NOT in FastMoveGenerator:\n";
    for (const auto& move : originalMoveSet) {
        if (fastMoveSet.find(move) == fastMoveSet.end()) {
            std::cout << "  " << move << "\n";
        }
    }
    
    std::cout << "\nAll FastMoveGenerator moves:\n";
    for (size_t i = 0; i < fastMoveStrings.size(); ++i) {
        std::cout << "  " << (i+1) << ". " << fastMoveStrings[i] << "\n";
    }
    
    std::cout << "\nAll Original moves:\n";
    for (size_t i = 0; i < originalMoveStrings.size(); ++i) {
        std::cout << "  " << (i+1) << ". " << originalMoveStrings[i] << "\n";
    }
    
    return 0;
}
