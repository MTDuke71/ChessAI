#include "src/Board.h"
#include "src/MoveGenerator.h"
#include "src/MoveEncoding.h"
#include <iostream>

int main() {
    std::cout << "Testing restored fixes...\n";
    
    // Test 1: Black castling encoding
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q1p/1PPBBPPP/R3K2R b KQkq - 0 1");
    
    std::cout << "Before O-O-O: King at " << __builtin_ctzll(board.getBlackKing()) << "\n";
    
    Board::MoveState state;
    board.makeMove("O-O-O", state);
    
    std::cout << "After O-O-O: King at " << __builtin_ctzll(board.getBlackKing()) << "\n";
    
    if (__builtin_ctzll(board.getBlackKing()) == 58) {
        std::cout << "✓ Black castling fix working\n";
    } else {
        std::cout << "✗ Black castling fix failed\n";
    }
    
    return 0;
}
