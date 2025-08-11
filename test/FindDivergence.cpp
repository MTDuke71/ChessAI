#include <iostream>
#include <set>
#include <string>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h"

// Depth-first search to find positions where move counts differ
void findDivergentPositions(Board& board, int currentDepth, int targetDepth, const std::string& moveSequence) {
    if (currentDepth == targetDepth) {
        FastMoveGenerator fastGen;
        MoveGenerator originalGen;
        
        bool isWhite = (currentDepth % 2) == 0;
        
        FastMoveGenerator::MoveList fastMoves;
        fastGen.generateLegalMoves(board, isWhite, fastMoves);
        
        std::vector<uint16_t> originalMoves = originalGen.generateLegalMoves(board, isWhite);
        
        if (fastMoves.count != originalMoves.size()) {
            std::cout << "DIVERGENCE at depth " << currentDepth << ":\n";
            std::cout << "Sequence: " << moveSequence << "\n";
            std::cout << "FEN: " << board.getFEN() << "\n";
            std::cout << "FastMoveGenerator: " << fastMoves.count << " moves\n";
            std::cout << "Original: " << originalMoves.size() << " moves\n";
            
            // Show the actual moves to identify the difference
            std::set<std::string> fastSet, originalSet;
            
            for (int i = 0; i < fastMoves.count; ++i) {
                fastSet.insert(fastMoves.moves[i].toAlgebraic());
            }
            
            for (const auto& move : originalMoves) {
                originalSet.insert(indexToAlgebraic(move));
            }
            
            std::cout << "Extra in FastMoveGenerator:\n";
            for (const auto& move : fastSet) {
                if (originalSet.find(move) == originalSet.end()) {
                    std::cout << "  " << move << "\n";
                }
            }
            
            std::cout << "Missing in FastMoveGenerator:\n";
            for (const auto& move : originalSet) {
                if (fastSet.find(move) == fastSet.end()) {
                    std::cout << "  " << move << "\n";
                }
            }
            std::cout << "\n";
            return; // Stop at first divergence for detailed analysis
        }
        return;
    }
    
    // Generate moves for current position
    FastMoveGenerator fastGen;
    FastMoveGenerator::MoveList moveList;
    bool isWhite = (currentDepth % 2) == 0;
    fastGen.generateLegalMoves(board, isWhite, moveList);
    
    // Try a limited number of moves to keep search manageable
    int moveCount = std::min(moveList.count, 3); // Only try first 3 moves to limit explosion
    
    for (int i = 0; i < moveCount; ++i) {
        auto move = moveList.moves[i];
        Board::MoveState state;
        
        std::string moveStr = move.toAlgebraic();
        board.makeMove(moveStr, state);
        
        findDivergentPositions(board, currentDepth + 1, targetDepth, 
                              moveSequence.empty() ? moveStr : moveSequence + " " + moveStr);
        
        board.unmakeMove(state);
    }
}

int main() {
    std::cout << "Finding Positions Where Move Generation Diverges\n";
    std::cout << "================================================\n\n";
    
    Board board;
    
    // Search for divergent positions at depth 3 and 4
    std::cout << "Searching for divergences at depth 3...\n";
    findDivergentPositions(board, 0, 3, "");
    
    std::cout << "Searching for divergences at depth 4...\n";
    findDivergentPositions(board, 0, 4, "");
    
    return 0;
}
