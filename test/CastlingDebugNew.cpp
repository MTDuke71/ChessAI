#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== TESTING isSquareAttacked ===\n";
    
    // Make a neutral white move that shouldn't affect black castling
    Board::MoveState state;
    board.makeMove("a2-a3", state);
    
    MoveGenerator gen;
    
    std::cout << "After a2-a3 (black to move):\n";
    std::cout << "Testing black castling requirements:\n";
    
    // Black kingside castling: e8-g8, need to check e8, f8, g8 not attacked by white
    std::cout << "  e8 attacked by white: " << gen.isSquareAttacked(board, 60, true) << "\n";
    std::cout << "  f8 attacked by white: " << gen.isSquareAttacked(board, 61, true) << "\n";
    std::cout << "  g8 attacked by white: " << gen.isSquareAttacked(board, 62, true) << "\n";
    
    // Black queenside castling: e8-c8, need to check e8, d8, c8 not attacked by white
    std::cout << "  d8 attacked by white: " << gen.isSquareAttacked(board, 59, true) << "\n";
    std::cout << "  c8 attacked by white: " << gen.isSquareAttacked(board, 58, true) << "\n";
    
    // Also check castling flags
    std::cout << "  Black can castle kingside (flag): " << board.canCastleBK() << "\n";
    std::cout << "  Black can castle queenside (flag): " << board.canCastleBQ() << "\n";
    
    // Check occupancy
    uint64_t occ = board.getWhitePieces() | board.getBlackPieces();
    std::cout << "  f8 occupied: " << ((occ & (1ULL << 61)) ? 1 : 0) << "\n";
    std::cout << "  g8 occupied: " << ((occ & (1ULL << 62)) ? 1 : 0) << "\n";
    std::cout << "  d8 occupied: " << ((occ & (1ULL << 59)) ? 1 : 0) << "\n";
    std::cout << "  c8 occupied: " << ((occ & (1ULL << 58)) ? 1 : 0) << "\n";
    std::cout << "  b8 occupied: " << ((occ & (1ULL << 57)) ? 1 : 0) << "\n";
    
    // Manual decode of the castling moves to see what they encode to
    std::cout << "\nCastling move encoding:\n";
    uint16_t ooMove = encodeMove("O-O");
    uint16_t oooMove = encodeMove("O-O-O");
    std::cout << "  O-O encodes to: from=" << moveFrom(ooMove) << " to=" << moveTo(ooMove) << " special=" << moveSpecial(ooMove) << "\n";
    std::cout << "  O-O-O encodes to: from=" << moveFrom(oooMove) << " to=" << moveTo(oooMove) << " special=" << moveSpecial(oooMove) << "\n";
    
    return 0;
}
