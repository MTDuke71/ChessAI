#include <iostream>
#include "Board.h"
#include "FastMoveGenerator.h"

int main() {
    std::cout << "Debugging FastMoveGenerator - Isolating the 35 Extra Nodes\n";
    std::cout << "==========================================================\n\n";
    
    Board board;
    FastMoveGenerator gen;
    
    // Test some specific positions that might reveal the bug
    // Check for common issues: en passant, castling, pawn promotion
    
    std::cout << "1. Testing starting position for any obvious issues:\n";
    FastMoveGenerator::MoveList moves;
    gen.generateLegalMoves(board, true, moves);
    
    std::cout << "Move count: " << moves.count << " (should be 20)\n";
    std::cout << "Moves generated:\n";
    for (int i = 0; i < moves.count; ++i) {
        std::cout << "  " << (i+1) << ". " << moves.moves[i].toAlgebraic() << "\n";
    }
    
    if (moves.count != 20) {
        std::cout << "ERROR: Starting position should have exactly 20 moves!\n";
        return 1;
    }
    
    std::cout << "\n2. Testing after one move (e2-e4):\n";
    Board::MoveState state;
    board.makeMove("e2-e4", state);
    
    FastMoveGenerator::MoveList blackMoves;
    gen.generateLegalMoves(board, false, blackMoves);
    
    std::cout << "Black move count after e2-e4: " << blackMoves.count << " (should be 20)\n";
    
    if (blackMoves.count != 20) {
        std::cout << "ERROR: Black should have 20 moves after e2-e4!\n";
        std::cout << "Black moves:\n";
        for (int i = 0; i < blackMoves.count; ++i) {
            std::cout << "  " << (i+1) << ". " << blackMoves.moves[i].toAlgebraic() << "\n";
        }
        return 1;
    }
    
    std::cout << "\nBasic move generation appears correct. The bug is deeper in the recursion.\n";
    std::cout << "Need to analyze specific move sequences that cause the 35-node discrepancy.\n";
    
    return 0;
}
