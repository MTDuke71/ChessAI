#include <iostream>
#include <vector>
#include <string>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h"
#include "Perft.h"

// Compare perft results at specific depth and identify mismatches
void comparePerftAtDepth(Board& board, int targetDepth, int currentDepth, const std::string& moveSequence) {
    if (currentDepth == targetDepth) {
        // At target depth, compare move counts
        FastMoveGenerator fastGen;
        MoveGenerator originalGen;
        
        FastMoveGenerator::MoveList fastMoves;
        fastGen.generateLegalMoves(board, currentDepth % 2 == targetDepth % 2, fastMoves);
        
        std::vector<uint16_t> originalMoves = originalGen.generateLegalMoves(board, currentDepth % 2 == targetDepth % 2);
        
        if (fastMoves.count != originalMoves.size()) {
            std::cout << "MISMATCH at position after moves: " << moveSequence << "\n";
            std::cout << "  FastMoveGenerator: " << fastMoves.count << " moves\n";
            std::cout << "  Original:          " << originalMoves.size() << " moves\n";
            
            // Print the board position
            std::cout << "  Board FEN: " << board.getFEN() << "\n\n";
        }
        return;
    }
    
    // Generate moves and recurse
    FastMoveGenerator fastGen;
    FastMoveGenerator::MoveList moveList;
    fastGen.generateLegalMoves(board, currentDepth % 2 == 0, moveList);
    
    for (int i = 0; i < moveList.count; ++i) {
        auto move = moveList.moves[i];
        Board::MoveState state;
        
        std::string moveStr = move.toAlgebraic();
        board.makeMove(moveStr, state);
        
        comparePerftAtDepth(board, targetDepth, currentDepth + 1, 
                           moveSequence.empty() ? moveStr : moveSequence + " " + moveStr);
        
        board.unmakeMove(state);
    }
}

int main() {
    std::cout << "Finding Exact Positions with Move Generation Mismatches\n";
    std::cout << "=======================================================\n\n";
    
    Board board;
    
    // Start from depth 3 and work our way up to find where mismatches begin
    for (int depth = 3; depth <= 4; ++depth) {
        std::cout << "Checking depth " << depth << " positions...\n";
        comparePerftAtDepth(board, depth, 0, "");
        std::cout << "Depth " << depth << " check complete.\n\n";
    }
    
    return 0;
}
