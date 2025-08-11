#include "../src/FastMoveGenerator.h"
#include "../src/Board.h"
#include <iostream>

int main() {
    Board board;
    FastMoveGenerator fastGen;
    
    // Test position: r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1
    board.loadFromFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    
    std::cout << "Testing castling position: r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1\n\n";
    
    // Test if key squares are under attack by black
    std::cout << "Square attack tests for WHITE CASTLING:\n";
    std::cout << "e1 (4) attacked by black: " << fastGen.isSquareAttacked(board, 4, false) << "\n";
    std::cout << "f1 (5) attacked by black: " << fastGen.isSquareAttacked(board, 5, false) << "\n";
    std::cout << "g1 (6) attacked by black: " << fastGen.isSquareAttacked(board, 6, false) << "\n";
    std::cout << "d1 (3) attacked by black: " << fastGen.isSquareAttacked(board, 3, false) << "\n";
    std::cout << "c1 (2) attacked by black: " << fastGen.isSquareAttacked(board, 2, false) << "\n";
    
    std::cout << "\nSquare attack tests for BLACK CASTLING:\n";
    std::cout << "e8 (60) attacked by white: " << fastGen.isSquareAttacked(board, 60, true) << "\n";
    std::cout << "f8 (61) attacked by white: " << fastGen.isSquareAttacked(board, 61, true) << "\n";
    std::cout << "g8 (62) attacked by white: " << fastGen.isSquareAttacked(board, 62, true) << "\n";
    std::cout << "d8 (59) attacked by white: " << fastGen.isSquareAttacked(board, 59, true) << "\n";
    std::cout << "c8 (58) attacked by white: " << fastGen.isSquareAttacked(board, 58, true) << "\n";
    
    // Check what pieces are on specific squares
    std::cout << "\nPiece positions:\n";
    std::cout << "Piece on e1 (4): " << (char)board.pieceAt(4) << "\n";
    std::cout << "Piece on a1 (0): " << (char)board.pieceAt(0) << "\n";
    std::cout << "Piece on h1 (7): " << (char)board.pieceAt(7) << "\n";
    std::cout << "Piece on e8 (60): " << (char)board.pieceAt(60) << "\n";
    std::cout << "Piece on a8 (56): " << (char)board.pieceAt(56) << "\n";
    std::cout << "Piece on h8 (63): " << (char)board.pieceAt(63) << "\n";
    
    // Check if castling rights are set
    std::cout << "\nCastling rights:\n";
    std::cout << "White can castle kingside: " << board.canCastleKingSide(true) << "\n";
    std::cout << "White can castle queenside: " << board.canCastleQueenSide(true) << "\n";
    std::cout << "Black can castle kingside: " << board.canCastleKingSide(false) << "\n";
    std::cout << "Black can castle queenside: " << board.canCastleQueenSide(false) << "\n";
    
    return 0;
}
