#include <iostream>
#include "Board.h"

int main() {
    std::cout << "Board Piece Position Test\n";
    std::cout << "========================\n\n";
    
    Board board;
    
    std::cout << "White Pawns bitboard: " << std::hex << board.getWhitePawns() << std::dec << "\n";
    std::cout << "Black Pawns bitboard: " << std::hex << board.getBlackPawns() << std::dec << "\n";
    
    // Check individual squares
    std::cout << "\nWhite pawn positions:\n";
    uint64_t whitePawns = board.getWhitePawns();
    for (int sq = 0; sq < 64; ++sq) {
        if (whitePawns & (1ULL << sq)) {
            int rank = sq / 8;
            int file = sq % 8;
            char fileChar = 'a' + file;
            char rankChar = '1' + rank;
            std::cout << "  " << fileChar << rankChar << " (square " << sq << ")\n";
        }
    }
    
    std::cout << "\nBlack pawn positions:\n";
    uint64_t blackPawns = board.getBlackPawns();
    for (int sq = 0; sq < 64; ++sq) {
        if (blackPawns & (1ULL << sq)) {
            int rank = sq / 8;
            int file = sq % 8;
            char fileChar = 'a' + file;
            char rankChar = '1' + rank;
            std::cout << "  " << fileChar << rankChar << " (square " << sq << ")\n";
        }
    }
    
    return 0;
}
