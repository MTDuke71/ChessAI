#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::cout << "=== DEBUGGING BOARD STATE ===\n";
    
    Board board;
    board.loadFEN("4k3/8/8/8/8/8/8/4K2R w K - 0 1");
    
    std::cout << "Position: 4k3/8/8/8/8/8/8/4K2R w K - 0 1\n";
    board.printBoard();
    
    uint64_t whitePieces = board.getWhitePieces();
    uint64_t blackPieces = board.getBlackPieces();
    uint64_t allPieces = whitePieces | blackPieces;
    
    std::cout << "\nSquare analysis:\n";
    std::cout << "e1 (4): " << ((whitePieces & (1ULL << 4)) ? "White piece" : "Empty") << "\n";
    std::cout << "f1 (5): " << ((allPieces & (1ULL << 5)) ? "Piece" : "Empty") << "\n";
    std::cout << "g1 (6): " << ((allPieces & (1ULL << 6)) ? "Piece" : "Empty") << "\n";
    std::cout << "h1 (7): " << ((whitePieces & (1ULL << 7)) ? "White piece" : "Empty") << "\n";
    
    std::cout << "\nCastling squares need to be empty for castling:\n";
    std::cout << "f1 empty: " << (!(allPieces & (1ULL << 5)) ? "YES" : "NO") << "\n";
    std::cout << "g1 empty: " << (!(allPieces & (1ULL << 6)) ? "YES" : "NO") << "\n";
    
    return 0;
}
