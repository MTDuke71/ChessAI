#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::cout << "Testing castling move generation fix..." << std::endl;
    
    // Test position: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1
    // After e5-f7, black should be able to castle kingside (e8-g8)
    
    Board board;
    board.loadFEN("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    
    // Make the e5-f7 move to get to the position we're analyzing
    board.makeMove(encodeMove("e5-f7"));
    
    std::cout << "Position after e5-f7:" << std::endl;
    board.printBoard();
    
    MoveGenerator gen;
    auto blackMoves = gen.generateAllMoves(board, false);
    
    std::cout << "\nBlack moves generated: " << blackMoves.size() << " moves" << std::endl;
    
    std::cout << "\nCastling rights check:" << std::endl;
    std::cout << "Black can castle kingside: " << (board.canCastleBK() ? "YES" : "NO") << std::endl;
    std::cout << "Black can castle queenside: " << (board.canCastleBQ() ? "YES" : "NO") << std::endl;
    
    // Check for specific castling moves by encoding them
    uint16_t kingsideCastling = encodeMove("e8-g8");
    uint16_t queensideCastling = encodeMove("e8-c8");
    
    bool foundKingsideCastling = false;
    bool foundQueensideCastling = false;
    
    for (const auto& move : blackMoves) {
        if (move == kingsideCastling) {
            foundKingsideCastling = true;
        }
        if (move == queensideCastling) {
            foundQueensideCastling = true;
        }
    }
    
    std::cout << "\nTest results:" << std::endl;
    std::cout << (foundKingsideCastling ? "✅" : "❌") << " Kingside castling (e8-g8) " 
              << (foundKingsideCastling ? "found" : "NOT found") << std::endl;
    std::cout << (foundQueensideCastling ? "✅" : "❌") << " Queenside castling (e8-c8) " 
              << (foundQueensideCastling ? "found" : "NOT found") << std::endl;
    
    // Expected is 2039 moves, we had 2036 before (missing 3 moves including the e8-g8)
    std::cout << "\nMove count: " << blackMoves.size() << " (expected: 2039)" << std::endl;
    
    return 0;
}
