#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>

int main() {
    Board board;
    MoveGenerator moveGen;
    
    // Test position without knights
    board.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    
    std::cout << "Testing position: r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1\n";
    std::cout << "(Note: This position has NO KNIGHTS)\n\n";
    
    // Generate moves with original system (white to move)
    std::vector<uint16_t> rawMoves = moveGen.generateLegalMoves(board, true);
    
    std::cout << "Original MoveGenerator: " << rawMoves.size() << " moves\n";
    for (size_t i = 0; i < rawMoves.size(); i++) {
        std::string moveStr = decodeMove(rawMoves[i]);
        std::cout << "  " << (i+1) << ". " << moveStr << "\n";
    }
    
    std::cout << "\nExpected moves:\n";
    std::cout << "  - 16 pawn moves (a2-a3, a2-a4, b2-b3, b2-b4, etc.)\n";
    std::cout << "  - 5 rook moves (Ra1-b1, Ra1-c1, Ra1-d1, Rh1-f1, Rh1-g1)\n";
    std::cout << "  - 2 king moves (Ke1-d1, Ke1-f1)\n";
    std::cout << "  - 2 castling moves (O-O, O-O-O)\n";
    std::cout << "  - Total expected: 25 moves\n";
    std::cout << "  - Actual: " << rawMoves.size() << " moves\n";
    
    return 0;
}
