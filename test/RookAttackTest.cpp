#include "../src/FastMoveGenerator.h"
#include "../src/Board.h"
#include "../src/Magic.h"
#include <iostream>

int main() {
    Board board;
    board.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    
    std::cout << "Debug rook attacks in position: r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1\n\n";
    
    uint64_t allPieces = board.getWhitePieces() | board.getBlackPieces();
    std::cout << "All pieces bitboard: 0x" << std::hex << allPieces << std::dec << "\n";
    
    // Check black rooks
    uint64_t blackRooks = board.getBlackRooks();
    std::cout << "Black rooks bitboard: 0x" << std::hex << blackRooks << std::dec << "\n";
    
    // Check if black rook on a8 (square 56) attacks e1 (square 4)
    uint64_t a8RookAttacks = Magic::getRookAttacks(56, allPieces);
    std::cout << "Black rook on a8 (56) attacks: 0x" << std::hex << a8RookAttacks << std::dec << "\n";
    std::cout << "Does a8 rook attack e1 (4)? " << ((a8RookAttacks & (1ULL << 4)) ? "YES" : "NO") << "\n";
    
    // Check if black rook on h8 (square 63) attacks e1 (square 4) 
    uint64_t h8RookAttacks = Magic::getRookAttacks(63, allPieces);
    std::cout << "Black rook on h8 (63) attacks: 0x" << std::hex << h8RookAttacks << std::dec << "\n";
    std::cout << "Does h8 rook attack e1 (4)? " << ((h8RookAttacks & (1ULL << 4)) ? "YES" : "NO") << "\n";
    
    // Check white rooks 
    uint64_t whiteRooks = board.getWhiteRooks();
    std::cout << "White rooks bitboard: 0x" << std::hex << whiteRooks << std::dec << "\n";
    
    // Check if white rook on a1 (square 0) attacks e8 (square 60)
    uint64_t a1RookAttacks = Magic::getRookAttacks(0, allPieces);
    std::cout << "White rook on a1 (0) attacks: 0x" << std::hex << a1RookAttacks << std::dec << "\n";
    std::cout << "Does a1 rook attack e8 (60)? " << ((a1RookAttacks & (1ULL << 60)) ? "YES" : "NO") << "\n";
    
    // Check if any black rook can attack the e1 square
    bool e1Attacked = false;
    if (blackRooks & a8RookAttacks & (1ULL << 4)) e1Attacked = true;
    if (blackRooks & h8RookAttacks & (1ULL << 4)) e1Attacked = true;
    
    std::cout << "\nCombined test: Is e1 attacked by black rooks? " << (e1Attacked ? "YES" : "NO") << "\n";
    
    return 0;
}
