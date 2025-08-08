#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::cout << "Analyzing position 3: 4k3/8/8/8/8/8/8/4K2R w K - 0 1" << std::endl;
    
    Board board;
    board.loadFEN("4k3/8/8/8/8/8/8/4K2R w K - 0 1");
    
    std::cout << "Board state:" << std::endl;
    board.printBoard();
    
    MoveGenerator gen;
    auto whiteMoves = gen.generateAllMoves(board, true);
    
    std::cout << "\nWhite moves generated: " << whiteMoves.size() << " moves (expected: 15)" << std::endl;
    
    std::cout << "\nCastling rights:" << std::endl;
    std::cout << "White can castle kingside: " << (board.canCastleWK() ? "YES" : "NO") << std::endl;
    std::cout << "White can castle queenside: " << (board.canCastleWQ() ? "YES" : "NO") << std::endl;
    
    // Check if castling move is present
    uint16_t kingsideCastling = encodeMove("e1-g1");
    bool foundCastling = false;
    
    for (const auto& move : whiteMoves) {
        if (move == kingsideCastling) {
            foundCastling = true;
            break;
        }
    }
    
    std::cout << "Kingside castling (e1-g1): " << (foundCastling ? "FOUND" : "NOT FOUND") << std::endl;
    
    return 0;
}
