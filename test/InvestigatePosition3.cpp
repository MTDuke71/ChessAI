#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include <iostream>

int main() {
    std::cout << "=== INVESTIGATING POSITION 3 DISCREPANCY ===\n";
    
    Board board;
    MoveGenerator generator;
    
    std::string fen = "4k3/8/8/8/8/8/8/4K2R w K - 0 1";
    
    std::cout << "FEN: " << fen << "\n";
    if (!board.loadFEN(fen)) {
        std::cerr << "Failed to load FEN\n";
        return 1;
    }
    
    std::cout << "Testing all depths to find where discrepancy starts:\n";
    
    uint64_t expected[] = {0, 15, 66, 1197, 7059, 133987, 764643};
    
    for (int depth = 1; depth <= 6; ++depth) {
        uint64_t nodes = perft(board, generator, depth);
        
        std::cout << "Depth " << depth << ": " << nodes 
                 << " (expected " << expected[depth] << ")";
        
        if (nodes == expected[depth]) {
            std::cout << " ✓\n";
        } else {
            int64_t diff = (int64_t)(nodes - expected[depth]);
            std::cout << " ✗ (diff: " << diff << ")\n";
            
            // If this is the first discrepancy, investigate further
            if (depth > 1) {
                std::cout << "First discrepancy found at depth " << depth << std::endl;
                return 0;
            }
        }
    }
    
    return 0;
}
