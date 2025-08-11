#include "../src/FastMoveGenerator.h"
#include "../src/Board.h"
#include <iostream>

int main() {
    Board board;
    FastMoveGenerator fastGen;
    
    // Test position: r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1
    board.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    
    std::cout << "Testing castling position: r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1\n\n";
    
    // Test if key squares are under attack by black
    std::cout << "WHITE CASTLING SQUARES (attacked by black?):\n";
    std::cout << "e1 (4) attacked by black: " << (fastGen.isSquareAttacked(board, 4, false) ? "YES" : "NO") << "\n";
    std::cout << "f1 (5) attacked by black: " << (fastGen.isSquareAttacked(board, 5, false) ? "YES" : "NO") << "\n";
    std::cout << "g1 (6) attacked by black: " << (fastGen.isSquareAttacked(board, 6, false) ? "YES" : "NO") << "\n";
    std::cout << "d1 (3) attacked by black: " << (fastGen.isSquareAttacked(board, 3, false) ? "YES" : "NO") << "\n";
    std::cout << "c1 (2) attacked by black: " << (fastGen.isSquareAttacked(board, 2, false) ? "YES" : "NO") << "\n";
    
    std::cout << "\nBLACK CASTLING SQUARES (attacked by white?):\n";
    std::cout << "e8 (60) attacked by white: " << (fastGen.isSquareAttacked(board, 60, true) ? "YES" : "NO") << "\n";
    std::cout << "f8 (61) attacked by white: " << (fastGen.isSquareAttacked(board, 61, true) ? "YES" : "NO") << "\n";
    std::cout << "g8 (62) attacked by white: " << (fastGen.isSquareAttacked(board, 62, true) ? "YES" : "NO") << "\n";
    std::cout << "d8 (59) attacked by white: " << (fastGen.isSquareAttacked(board, 59, true) ? "YES" : "NO") << "\n";
    std::cout << "c8 (58) attacked by white: " << (fastGen.isSquareAttacked(board, 58, true) ? "YES" : "NO") << "\n";
    
    // Check castling rights
    std::cout << "\nCASTLING RIGHTS:\n";
    std::cout << "White can castle kingside: " << (board.canCastleWK() ? "YES" : "NO") << "\n";
    std::cout << "White can castle queenside: " << (board.canCastleWQ() ? "YES" : "NO") << "\n";
    std::cout << "Black can castle kingside: " << (board.canCastleBK() ? "YES" : "NO") << "\n";
    std::cout << "Black can castle queenside: " << (board.canCastleBQ() ? "YES" : "NO") << "\n";
    
    return 0;
}
