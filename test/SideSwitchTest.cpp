#include "../src/Board.h"
#include <iostream>

int main() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== TESTING SIDE SWITCHING ===\n";
    std::cout << "Initial: White to move = " << board.isWhiteToMove() << " (should be 1)\n";
    
    // Make a simple white move
    Board::MoveState state;
    std::cout << "Making move: a2-a3\n";
    board.makeMove("a2-a3", state);
    
    std::cout << "After a2-a3: White to move = " << board.isWhiteToMove() << " (should be 0)\n";
    
    // Try another move
    std::cout << "Making move: a7-a6 (black's turn)\n";
    board.makeMove("a7-a6", state);  // This should fail if white to move is wrong
    
    std::cout << "After a7-a6: White to move = " << board.isWhiteToMove() << " (should be 1)\n";
    
    return 0;
}
