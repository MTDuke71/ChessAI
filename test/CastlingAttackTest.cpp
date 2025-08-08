#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

bool isSquareUnderAttack(const Board& board, int square, bool byWhite) {
    uint64_t attackBitboard = byWhite ? board.getWhiteAttacks() : board.getBlackAttacks();
    return (attackBitboard & (1ULL << square)) != 0;
}

int main() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== TESTING BLACK CASTLING LEGALITY ===\n";
    
    // Make a neutral white move that shouldn't affect black castling
    Board::MoveState state;
    board.makeMove("a2-a3", state);
    
    std::cout << "After a2-a3 (black to move):\n";
    
    // Check if black king/castling squares are under attack by white
    std::cout << "  e8 (king) under attack by white: " << (isSquareUnderAttack(board, 60, true) ? "YES" : "NO") << "\n";
    std::cout << "  f8 under attack by white: " << (isSquareUnderAttack(board, 61, true) ? "YES" : "NO") << "\n";
    std::cout << "  g8 under attack by white: " << (isSquareUnderAttack(board, 62, true) ? "YES" : "NO") << "\n";
    std::cout << "  c8 under attack by white: " << (isSquareUnderAttack(board, 58, true) ? "YES" : "NO") << "\n";
    std::cout << "  d8 under attack by white: " << (isSquareUnderAttack(board, 59, true) ? "YES" : "NO") << "\n";
    
    // Let's also test with the original position (white to move)
    Board originalBoard;
    originalBoard.loadFEN(fen);
    
    std::cout << "\nOriginal position (white to move):\n";
    std::cout << "  e1 (king) under attack by black: " << (isSquareUnderAttack(originalBoard, 4, false) ? "YES" : "NO") << "\n";
    std::cout << "  f1 under attack by black: " << (isSquareUnderAttack(originalBoard, 5, false) ? "YES" : "NO") << "\n";
    std::cout << "  g1 under attack by black: " << (isSquareUnderAttack(originalBoard, 6, false) ? "YES" : "NO") << "\n";
    std::cout << "  c1 under attack by black: " << (isSquareUnderAttack(originalBoard, 2, false) ? "YES" : "NO") << "\n";
    std::cout << "  d1 under attack by black: " << (isSquareUnderAttack(originalBoard, 3, false) ? "YES" : "NO") << "\n";
    
    // Test if white castling is legal
    std::cout << "  White O-O legal: " << originalBoard.isMoveLegal("O-O") << "\n";
    std::cout << "  White O-O-O legal: " << originalBoard.isMoveLegal("O-O-O") << "\n";
    
    return 0;
}
