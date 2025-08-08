#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include <iostream>

int main() {
    std::cout << "=== PERFT DIVIDE ANALYSIS FOR POSITION 3 ===\n";
    
    Board board;
    MoveGenerator generator;
    
    std::string fen = "4k3/8/8/8/8/8/8/4K2R w K - 0 1";
    
    if (!board.loadFEN(fen)) {
        std::cerr << "Failed to load FEN\n";
        return 1;
    }
    
    std::cout << "Position: " << fen << "\n";
    std::cout << "Performing perft divide at depth 5 (each move's depth 4 continuation)\n\n";
    
    auto moves = generator.generateAllMoves(board, true);
    
    uint64_t totalNodes = 0;
    
    for (const auto& move : moves) {
        Board tempBoard = board;
        if (tempBoard.makeMove(move)) {
            uint64_t nodes = perft(tempBoard, generator, 4);  // depth 5 total = 1 + 4
            totalNodes += nodes;
            
            // Try to decode the move for display
            std::cout << "Move " << move << ": " << nodes << " nodes\n";
        }
    }
    
    std::cout << "\nTotal nodes at depth 5: " << totalNodes << " (expected: 133987)\n";
    std::cout << "Difference: " << (int64_t)(totalNodes - 133987) << "\n";
    
    return 0;
}
