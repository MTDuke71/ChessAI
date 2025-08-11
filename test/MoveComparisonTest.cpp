#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h"
#include "MoveEncoding.h"

int main() {
    std::cout << "Move Generation Comparison Analysis\n";
    std::cout << "===================================\n\n";
    
    Board board;
    FastMoveGenerator fastGen;
    MoveGenerator originalGen;
    
    // Generate moves using both generators
    FastMoveGenerator::MoveList fastMoves;
    fastGen.generateLegalMoves(board, true, fastMoves);
    
    std::vector<uint16_t> originalMoves = originalGen.generateLegalMoves(board, true);
    
    // Convert to sets of strings for comparison
    std::set<std::string> fastMoveSet;
    std::set<std::string> originalMoveSet;
    
    for (int i = 0; i < fastMoves.count; ++i) {
        fastMoveSet.insert(fastMoves.moves[i].toAlgebraic());
    }
    
    for (const auto& move : originalMoves) {
        originalMoveSet.insert(decodeMove(move));
    }
    
    std::cout << "FastMoveGenerator: " << fastMoves.count << " moves\n";
    std::cout << "Original MoveGenerator: " << originalMoves.size() << " moves\n\n";
    
    // Find moves only in FastMoveGenerator (extra moves)
    std::vector<std::string> extraMoves;
    std::set_difference(fastMoveSet.begin(), fastMoveSet.end(),
                       originalMoveSet.begin(), originalMoveSet.end(),
                       std::back_inserter(extraMoves));
    
    // Find moves only in Original (missing moves)
    std::vector<std::string> missingMoves;
    std::set_difference(originalMoveSet.begin(), originalMoveSet.end(),
                       fastMoveSet.begin(), fastMoveSet.end(),
                       std::back_inserter(missingMoves));
    
    if (!extraMoves.empty()) {
        std::cout << "EXTRA moves in FastMoveGenerator (" << extraMoves.size() << "):\n";
        for (const auto& move : extraMoves) {
            std::cout << "  " << move << "\n";
        }
        std::cout << "\n";
    }
    
    if (!missingMoves.empty()) {
        std::cout << "MISSING moves from FastMoveGenerator (" << missingMoves.size() << "):\n";
        for (const auto& move : missingMoves) {
            std::cout << "  " << move << "\n";
        }
        std::cout << "\n";
    }
    
    if (extraMoves.empty() && missingMoves.empty()) {
        std::cout << "✅ PERFECT MATCH! Both generators produce identical moves.\n";
    } else {
        std::cout << "❌ MISMATCH! Net difference: " << (int)fastMoves.count - (int)originalMoves.size() << " moves\n";
    }
    
    return 0;
}
