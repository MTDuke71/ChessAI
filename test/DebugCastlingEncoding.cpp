#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::cout << "=== DEBUGGING CASTLING MOVE ENCODING ===\n";
    
    Board board;
    board.loadFEN("4k3/8/8/8/8/8/8/4K2R w K - 0 1");
    
    uint16_t castlingMove = encodeMove("e1-g1");
    
    std::cout << "Castling move e1-g1 encoded as: " << castlingMove << "\n";
    
    // Extract move components
    int from = (castlingMove >> 6) & 0x3F;
    int to = castlingMove & 0x3F;
    int special = (castlingMove >> 14) & 0x03;  // Fixed: should be >> 14, not >> 12
    
    std::cout << "From: " << from << " (e1 should be 4)\n";
    std::cout << "To: " << to << " (g1 should be 6)\n";
    std::cout << "Special: " << special << " (castling should be 3)\n";
    
    std::cout << "\nTesting isMoveLegal:\n";
    bool legal = board.isMoveLegal(castlingMove);
    std::cout << "e1-g1 is " << (legal ? "LEGAL" : "ILLEGAL") << "\n";
    
    if (!legal) {
        std::cout << "\nChecking castling conditions:\n";
        std::cout << "Can castle WK: " << board.canCastleWK() << "\n";
        std::cout << "From square (4): " << ((from == 4) ? "OK" : "WRONG") << "\n";
        std::cout << "To square (6): " << ((to == 6) ? "OK" : "WRONG - expected 6, got " + std::to_string(to)) << "\n";
        std::cout << "Special flag (3): " << ((special == 3) ? "OK" : "WRONG - expected 3, got " + std::to_string(special)) << "\n";
    }
    
    return 0;
}
