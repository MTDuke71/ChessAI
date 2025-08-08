#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

void debugG2G4() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== DEBUGGING g2-g4 EN PASSANT ===\n";
    
    Board copy = board;
    Board::MoveState state;
    copy.makeMove("g2-g4", state);
    
    std::cout << "After g2-g4:\n";
    copy.printBoard();
    std::cout << "En passant square: " << copy.getEnPassantSquare() << "\n";
    
    // Check if there's a black pawn on h3 that should be able to capture en passant
    uint64_t blackPawns = copy.getBlackPawns();
    if (blackPawns & (1ULL << 23)) {  // h3 = square 23
        std::cout << "Black pawn on h3 found!\n";
        std::cout << "Should be able to capture on g3 (square 22)\n";
        
        // Manually check the en passant logic
        int epSquare = copy.getEnPassantSquare();
        std::cout << "Expected en passant square: 22 (g3)\n";
        std::cout << "Actual en passant square: " << epSquare << "\n";
        
        if (epSquare != 22) {
            std::cout << "❌ BUG: En passant square not set correctly for g2-g4!\n";
        }
    }
    
    // Check what the applyMove function does for g2-g4
    std::cout << "\n=== CHECKING MOVE APPLICATION ===\n";
    Board test = board;
    std::cout << "Before g2-g4 - en passant square: " << test.getEnPassantSquare() << "\n";
    
    // The issue might be in how applyMove sets the en passant square
}

int main() {
    debugG2G4();
    return 0;
}
