#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h"
#include "MoveEncoding.h"

void compareMoveGeneration(Board& board, bool isWhite, const std::string& position) {
    FastMoveGenerator fastGen;
    MoveGenerator originalGen;
    
    // Generate moves using both generators
    FastMoveGenerator::MoveList fastMoves;
    fastGen.generateLegalMoves(board, isWhite, fastMoves);
    
    std::vector<uint16_t> originalMoves = originalGen.generateLegalMoves(board, isWhite);
    
    // Convert to sets of strings for comparison
    std::set<std::string> fastMoveSet;
    std::set<std::string> originalMoveSet;
    
    for (int i = 0; i < fastMoves.count; ++i) {
        fastMoveSet.insert(fastMoves.moves[i].toAlgebraic());
    }
    
    for (const auto& move : originalMoves) {
        originalMoveSet.insert(decodeMove(move));
    }
    
    std::cout << "Position: " << position << "\n";
    std::cout << "FastMoveGenerator: " << fastMoves.count << " moves\n";
    std::cout << "Original MoveGenerator: " << originalMoves.size() << " moves\n";
    
    // Find differences
    std::vector<std::string> extraMoves;
    std::set_difference(fastMoveSet.begin(), fastMoveSet.end(),
                       originalMoveSet.begin(), originalMoveSet.end(),
                       std::back_inserter(extraMoves));
    
    std::vector<std::string> missingMoves;
    std::set_difference(originalMoveSet.begin(), originalMoveSet.end(),
                       fastMoveSet.begin(), fastMoveSet.end(),
                       std::back_inserter(missingMoves));
    
    if (!extraMoves.empty()) {
        std::cout << "EXTRA moves in FastMoveGenerator:\n";
        for (const auto& move : extraMoves) {
            std::cout << "  " << move << "\n";
        }
    }
    
    if (!missingMoves.empty()) {
        std::cout << "MISSING moves from FastMoveGenerator:\n";
        for (const auto& move : missingMoves) {
            std::cout << "  " << move << "\n";
        }
    }
    
    if (extraMoves.empty() && missingMoves.empty()) {
        std::cout << "✅ Perfect match\n";
    } else {
        std::cout << "❌ Mismatch: +" << extraMoves.size() << " extra, -" << missingMoves.size() << " missing\n";
    }
    std::cout << "\n";
}

int main() {
    std::cout << "Deep Move Generation Analysis\n";
    std::cout << "=============================\n\n";
    
    Board board;
    
    // Test starting position
    compareMoveGeneration(board, true, "Starting position (White)");
    
    // Make some moves and test at depth 2
    std::vector<std::string> testMoves = {"e2-e4", "e7-e5", "g1-f3", "b8-c6"};
    
    for (const auto& moveStr : testMoves) {
        Board::MoveState state;
        board.makeMove(moveStr, state);
        
        bool isWhite = (moveStr == "e2-e4" || moveStr == "g1-f3") ? false : true; // Next player
        std::string position = "After " + moveStr + " (" + (isWhite ? "White" : "Black") + " to move)";
        compareMoveGeneration(board, isWhite, position);
    }
    
    return 0;
}
