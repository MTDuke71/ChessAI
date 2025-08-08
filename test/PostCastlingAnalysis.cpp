#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>

int main() {
    std::cout << "=== ANALYZING POST-CASTLING POSITION ===\n";
    
    Board board;
    MoveGenerator generator;
    
    // Start with position 3
    std::string fen = "4k3/8/8/8/8/8/8/4K2R w K - 0 1";
    
    if (!board.loadFEN(fen)) {
        std::cerr << "Failed to load FEN\n";
        return 1;
    }
    
    std::cout << "Original position: " << fen << "\n";
    
    // Make the castling move
    board.makeMove(encodeMove("e1-g1"));
    
    std::cout << "After castling e1-g1:\n";
    board.printBoard();
    
    std::cout << "Castling rights after castling:\n";
    std::cout << "White KS: " << (board.canCastleWK() ? "YES" : "NO") << "\n";
    std::cout << "White QS: " << (board.canCastleWQ() ? "YES" : "NO") << "\n";
    
    // Test perft from this position
    std::cout << "\nPerft results from post-castling position:\n";
    for (int depth = 1; depth <= 3; ++depth) {
        uint64_t nodes = perft(board, generator, depth);
        std::cout << "Depth " << depth << ": " << nodes << " nodes\n";
    }
    
    return 0;
}
