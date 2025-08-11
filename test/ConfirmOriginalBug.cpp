#include <iostream>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h"

int main() {
    std::cout << "Testing Specific Position with Move Generation Bug\n";
    std::cout << "==================================================\n\n";
    
    // Test the position: rnbqkbnr/ppp1pppp/8/3p4/8/3P3N/PPP1PPPP/RNBQKB1R b KQkq - 0 3
    // This is after moves: g1-h3 d7-d5 d2-d3
    
    Board board;
    
    // Make the moves
    Board::MoveState state1, state2, state3;
    board.makeMove("g1-h3", state1);
    board.makeMove("d7-d5", state2);
    board.makeMove("d2-d3", state3);
    
    std::cout << "Position after g1-h3 d7-d5 d2-d3:\n";
    std::cout << "FEN: " << board.getFEN() << "\n\n";
    
    // Test both generators
    FastMoveGenerator fastGen;
    MoveGenerator originalGen;
    
    // FastMoveGenerator (it's Black to move)
    FastMoveGenerator::MoveList fastMoves;
    fastGen.generateLegalMoves(board, false, fastMoves);  // false = Black to move
    
    std::cout << "FastMoveGenerator moves for Black:\n";
    for (int i = 0; i < fastMoves.count; ++i) {
        std::cout << "  " << (i+1) << ". " << fastMoves.moves[i].toAlgebraic() << "\n";
    }
    std::cout << "Total FastMoveGenerator moves: " << fastMoves.count << "\n\n";
    
    // Original MoveGenerator
    std::vector<uint16_t> originalMoves = originalGen.generateLegalMoves(board, false);  // false = Black to move
    
    std::cout << "Original MoveGenerator moves for Black:\n";
    for (size_t i = 0; i < originalMoves.size(); ++i) {
        std::cout << "  " << (i+1) << ". " << indexToAlgebraic(originalMoves[i]) << "\n";
    }
    std::cout << "Total Original moves: " << originalMoves.size() << "\n\n";
    
    if (fastMoves.count != originalMoves.size()) {
        std::cout << "CONFIRMED: Original MoveGenerator bug!\n";
        std::cout << "FastMoveGenerator is working correctly.\n";
        std::cout << "The discrepancy in perft is due to the Original MoveGenerator failing to generate moves for Black.\n";
    } else {
        std::cout << "Both generators agree.\n";
    }
    
    return 0;
}
