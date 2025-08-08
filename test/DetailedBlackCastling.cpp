#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::cout << "=== DETAILED BLACK CASTLING DEBUG ===\n";
    
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q1p/1PPBBPPP/R3K2R b KQkq - 0 1");
    
    std::cout << "BEFORE castling:\n";
    std::cout << "Black king: " << std::hex << board.getBlackKing() << std::dec 
              << " (position: " << __builtin_ctzll(board.getBlackKing()) << ")\n";
    std::cout << "Black rooks: " << std::hex << board.getBlackRooks() << std::dec << "\n";
    
    // Check if the king is where we expect (e8 = square 60)
    uint64_t e8 = 1ULL << 60;
    if (board.getBlackKing() & e8) {
        std::cout << "✓ King is on e8 before castling\n";
    } else {
        std::cout << "✗ King is NOT on e8 before castling!\n";
    }
    
    // Check rooks before castling (should be on a8=56 and h8=63)
    uint64_t a8 = 1ULL << 56;
    uint64_t h8 = 1ULL << 63;
    if (board.getBlackRooks() & a8) {
        std::cout << "✓ Rook is on a8 before castling\n";
    } else {
        std::cout << "✗ Rook is NOT on a8 before castling!\n";
    }
    
    // Now make the move
    Board::MoveState state;
    std::cout << "\nMaking move: O-O-O (black queenside castling)\n";
    board.makeMove("O-O-O", state);
    
    std::cout << "\nAFTER castling:\n";
    std::cout << "Black king: " << std::hex << board.getBlackKing() << std::dec 
              << " (position: " << __builtin_ctzll(board.getBlackKing()) << ")\n";
    std::cout << "Black rooks: " << std::hex << board.getBlackRooks() << std::dec << "\n";
    
    // Check final positions
    uint64_t c8 = 1ULL << 58;  // Expected king position
    uint64_t d8 = 1ULL << 59;  // Expected rook position
    
    if (board.getBlackKing() & c8) {
        std::cout << "✓ King correctly moved to c8\n";
    } else {
        std::cout << "✗ King NOT on c8! Actual position: " << __builtin_ctzll(board.getBlackKing()) << "\n";
    }
    
    if (board.getBlackRooks() & d8) {
        std::cout << "✓ Rook correctly moved to d8\n";
    } else {
        std::cout << "✗ Rook NOT on d8!\n";
    }
    
    // Check that a8 rook is gone
    if (!(board.getBlackRooks() & a8)) {
        std::cout << "✓ Rook correctly removed from a8\n";
    } else {
        std::cout << "✗ Rook still on a8!\n";
    }
    
    return 0;
}
