#include <iostream>
#include "../src/Board.h"
#include "../src/Engine.h"

int main() {
    std::cout << "=== DIRECT ENGINE CASTLING TEST ===\n";
    
    Board board;
    // Position where white can castle kingside
    board.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    
    Engine engine;
    std::string bestMove = engine.searchBestMove(board, 1);
    
    std::cout << "Position: r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1\n";
    std::cout << "Best move: " << bestMove << "\n";
    
    // Test UCI conversion manually
    if (bestMove == "O-O") {
        std::cout << "UCI conversion: e1g1 (white kingside castling)\n";
        std::cout << "✅ Fix should work!\n";
    } else if (bestMove == "O-O-O") {
        std::cout << "UCI conversion: e1c1 (white queenside castling)\n";
        std::cout << "✅ Fix should work!\n";
    } else {
        std::cout << "Engine chose: " << bestMove << " (not castling)\n";
    }
    
    return 0;
}
