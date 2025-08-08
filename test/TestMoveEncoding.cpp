#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::cout << "=== TESTING MOVE ENCODING ISSUES ===\n";
    
    // Test some regular moves that might be incorrectly flagged as castling
    std::vector<std::string> testMoves = {
        "e5-f7",  // Should be normal move
        "e1-d1",  // Should be normal king move
        "e1-f1",  // Should be normal king move
        "e1-g1",  // Could be castling OR normal king move
        "e1-c1",  // Could be castling OR normal king move
        "e8-g8",  // Could be castling OR normal king move
        "e8-c8"   // Could be castling OR normal king move
    };
    
    for (const auto& move : testMoves) {
        uint16_t encoded = encodeMove(move, true); // White to move
        int special = (encoded >> 14) & 0x3;
        
        std::cout << "Move: " << move 
                 << " | Encoded: " << encoded 
                 << " | Special: " << special 
                 << " | " << (special == 3 ? "CASTLING" : "NORMAL") << "\n";
    }
    
    return 0;
}
