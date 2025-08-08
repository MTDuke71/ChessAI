#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include <iostream>

int main() {
    std::cout << "=== TESTING POSITION 2 FROM PERFTSUITE ===\n";
    
    Board board;
    MoveGenerator generator;
    
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    
    std::cout << "FEN: " << fen << "\n";
    if (!board.loadFEN(fen)) {
        std::cerr << "Failed to load FEN\n";
        return 1;
    }
    
    std::cout << "Testing different depths:\n";
    
    for (int depth = 1; depth <= 3; ++depth) {
        uint64_t nodes = perft(board, generator, depth);
        
        uint64_t expected = 0;
        switch(depth) {
            case 1: expected = 48; break;
            case 2: expected = 2039; break;
            case 3: expected = 97862; break;
        }
        
        std::cout << "Depth " << depth << ": " << nodes 
                 << " (expected " << expected << ")";
        
        if (nodes == expected) {
            std::cout << " ✓\n";
        } else {
            std::cout << " ✗ (diff: " << (int64_t)(nodes - expected) << ")\n";
        }
    }
    
    return 0;
}
