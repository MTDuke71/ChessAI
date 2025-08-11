#include "../src/Board.h"
#include "../src/FastMoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    Board board;
    FastMoveGenerator fastGen;
    
    // Load position after d2-d3
    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/3P4/PPP1PPPP/RNBQKBNR b KQkq - 0 2");
    
    std::cout << "DEBUGGING FAST MOVE GENERATOR\n";
    std::cout << "============================\n\n";
    
    // Generate black's moves
    FastMoveGenerator::MoveList blackMoves;
    fastGen.generateLegalMoves(board, false, blackMoves);
    
    std::cout << "Generated " << blackMoves.count << " moves:\n";
    
    for (int i = 0; i < blackMoves.count; i++) {
        auto& move = blackMoves.moves[i];
        std::cout << "Move " << (i+1) << ": ";
        std::cout << "from=" << move.from() << " to=" << move.to() << " ";
        std::cout << "piece=" << move.piece() << " ";
        std::cout << "capture=" << move.isCapture() << " ";
        std::cout << "algebraic='" << move.toAlgebraic() << "'\n";
        
        // Try to make the move and see what happens
        Board::MoveState state;
        std::string moveStr = move.toAlgebraic();
        board.makeMove(moveStr, state);
        std::cout << "  After move: " << board.getFEN() << "\n";
        board.unmakeMove(state);
        std::cout << "  After undo: " << board.getFEN() << "\n\n";
        
        if (i >= 5) {
            std::cout << "... (showing first 5 moves only)\n";
            break;
        }
    }
    
    return 0;
}
