#include "../src/Board.h"
#include <iostream>

int main() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== POSITION ANALYSIS ===\n";
    std::cout << "FEN: " << fen << "\n\n";
    
    // Check what's on the first rank
    std::cout << "First rank pieces:\n";
    for (int file = 0; file < 8; file++) {
        int square = file;  // rank 0 (1st rank)
        char filename = 'a' + file;
        
        uint64_t squareMask = 1ULL << square;
        std::string piece = "empty";
        
        if (board.getWhitePawns() & squareMask) piece = "White Pawn";
        else if (board.getWhiteRooks() & squareMask) piece = "White Rook";
        else if (board.getWhiteKnights() & squareMask) piece = "White Knight";
        else if (board.getWhiteBishops() & squareMask) piece = "White Bishop";
        else if (board.getWhiteQueens() & squareMask) piece = "White Queen";
        else if (board.getWhiteKing() & squareMask) piece = "White King";
        else if (board.getBlackPawns() & squareMask) piece = "Black Pawn";
        else if (board.getBlackRooks() & squareMask) piece = "Black Rook";
        else if (board.getBlackKnights() & squareMask) piece = "Black Knight";
        else if (board.getBlackBishops() & squareMask) piece = "Black Bishop";
        else if (board.getBlackQueens() & squareMask) piece = "Black Queen";
        else if (board.getBlackKing() & squareMask) piece = "Black King";
        
        std::cout << filename << "1: " << piece << "\n";
    }
    
    std::cout << "\nChecking potential issues with e1-d1:\n";
    
    // Check if king is currently in check
    std::cout << "1. Is white king currently in check? ";
    // We'd need to implement this check, but let's assume not for now
    std::cout << "TBD\n";
    
    // Check if e1-d1 would expose the king to check through discovered attack
    std::cout << "2. Would e1-d1 create a discovered check? ";
    // This would need more analysis
    std::cout << "TBD\n";
    
    // The main issue might be that our reference uses a different move generation 
    // or has a different interpretation of this position
    
    return 0;
}
