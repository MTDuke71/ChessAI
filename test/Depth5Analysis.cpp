#include <iostream>
#include <vector>
#include <string>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h"
#include "Perft.h"

uint64_t fastPerft(Board& board, FastMoveGenerator& gen, int depth, bool isWhite, const std::string& prefix = "") {
    if (depth == 0) return 1ULL;
    
    FastMoveGenerator::MoveList moveList;
    gen.generateLegalMoves(board, isWhite, moveList);
    
    if (depth == 1) {
        return moveList.count;
    }
    
    uint64_t nodes = 0ULL;
    for (int i = 0; i < moveList.count; ++i) {
        auto move = moveList.moves[i];
        Board::MoveState state;
        
        std::string moveStr = move.toAlgebraic();
        board.makeMove(moveStr, state);
        uint64_t childNodes = fastPerft(board, gen, depth - 1, !isWhite, prefix + moveStr + " ");
        nodes += childNodes;
        
        if (depth == 5 && prefix.empty()) {
            std::cout << "Move " << moveStr << ": " << childNodes << " nodes\n";
        }
        
        board.unmakeMove(state);
    }
    return nodes;
}

int main() {
    std::cout << "Depth 5 Analysis - FastMoveGenerator vs Original\n";
    std::cout << "=================================================\n\n";
    
    Board board;
    FastMoveGenerator fastGen;
    MoveGenerator originalGen;
    
    // Test depth 5 with detailed output
    std::cout << "Depth 5 analysis:\n";
    uint64_t fastNodes = fastPerft(board, fastGen, 5, true);
    uint64_t originalNodes = perft(board, originalGen, 5);
    
    std::cout << "\nSummary:\n";
    std::cout << "FastMoveGenerator: " << fastNodes << "\n";
    std::cout << "Original Perft:    " << originalNodes << "\n";
    std::cout << "Difference:        " << (int64_t)(fastNodes - originalNodes) << "\n";
    
    return 0;
}
