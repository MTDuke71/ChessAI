#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::cout << "Testing white castling in position: 4k3/8/8/8/8/8/8/4K2R w K - 0 1" << std::endl;
    
    Board board;
    board.loadFEN("4k3/8/8/8/8/8/8/4K2R w K - 0 1");
    
    std::cout << "Position:" << std::endl;
    board.printBoard();
    
    MoveGenerator gen;
    auto whiteMoves = gen.generateAllMoves(board, true);
    
    std::cout << "\nWhite moves generated: " << whiteMoves.size() << " moves (expected: 15)" << std::endl;
    
    std::cout << "\nCastling rights check:" << std::endl;
    std::cout << "White can castle kingside: " << (board.canCastleWK() ? "YES" : "NO") << std::endl;
    std::cout << "White can castle queenside: " << (board.canCastleWQ() ? "YES" : "NO") << std::endl;
    
    // Check for specific castling moves by encoding them  
    uint16_t kingsideCastling = encodeMove("e1-g1");
    
    bool foundKingsideCastling = false;
    
    for (const auto& move : whiteMoves) {
        if (move == kingsideCastling) {
            foundKingsideCastling = true;
            break;
        }
    }
    
    std::cout << "\nTest results:" << std::endl;
    std::cout << (foundKingsideCastling ? "✅" : "❌") << " Kingside castling (e1-g1) " 
              << (foundKingsideCastling ? "found" : "NOT found") << std::endl;
    
    return 0;
}
