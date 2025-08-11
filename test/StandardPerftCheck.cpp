#include <iostream>
#include "Board.h"
#include "FastMoveGenerator.h"

// Use the same perft methodology as the original, but with FastMoveGenerator
uint64_t fastPerftStandard(Board& board, int depth, bool isWhite) {
    if (depth == 0) return 1ULL;
    
    FastMoveGenerator gen;
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
        nodes += fastPerftStandard(board, depth - 1, !isWhite);
        board.unmakeMove(state);
    }
    return nodes;
}

int main() {
    std::cout << "FastMoveGenerator vs Known Perft Results\n";
    std::cout << "========================================\n\n";
    
    Board board;
    
    // Known correct perft results for starting position from chess.com perft calculator
    uint64_t knownResults[] = {
        1,           // depth 0
        20,          // depth 1  
        400,         // depth 2
        8902,        // depth 3
        197281,      // depth 4
        4865609,     // depth 5
        119060324    // depth 6
    };
    
    for (int depth = 1; depth <= 6; ++depth) {
        std::cout << "Depth " << depth << ":\n";
        
        uint64_t result = fastPerftStandard(board, depth, true);
        uint64_t expected = knownResults[depth];
        
        std::cout << "  Result:   " << result << "\n";
        std::cout << "  Expected: " << expected << "\n";
        
        if (result == expected) {
            std::cout << "  ✅ CORRECT\n";
        } else {
            std::cout << "  ❌ ERROR! Difference: " << (int64_t)(result - expected) << "\n";
            std::cout << "  FastMoveGenerator has a bug that needs fixing.\n";
            break;
        }
        std::cout << "\n";
    }
    
    return 0;
}
