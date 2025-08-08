#include "../src/Board.h"
#include <iostream>

int main() {
    Board board;
    board.loadFEN("4R3/8/3k4/8/8/8/8/4K3 w - - 4 3");
    
    std::cout << "=== PIECE IDENTIFICATION ===" << std::endl;
    std::cout << "Position: 4R3/8/3k4/8/8/8/8/4K3 w - - 4 3" << std::endl;
    std::cout << std::endl;
    
    int e8_square = 60; // e8
    std::cout << "Square e8 (index " << e8_square << "):" << std::endl;
    
    auto color = board.pieceColorAt(e8_square);
    if (color == Board::Color::White) {
        std::cout << "  Color: White" << std::endl;
    } else if (color == Board::Color::Black) {
        std::cout << "  Color: Black" << std::endl;
    } else {
        std::cout << "  Color: None (empty)" << std::endl;
    }
    
    // Check what piece is on e8
    uint64_t square_mask = 1ULL << e8_square;
    if (board.getWhiteRooks() & square_mask) {
        std::cout << "  Piece: White Rook" << std::endl;
    } else if (board.getBlackRooks() & square_mask) {
        std::cout << "  Piece: Black Rook" << std::endl;
    } else if (board.getWhiteKing() & square_mask) {
        std::cout << "  Piece: White King" << std::endl;
    } else if (board.getBlackKing() & square_mask) {
        std::cout << "  Piece: Black King" << std::endl;
    } else {
        std::cout << "  Piece: None (empty square)" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== THE BUG EXPLANATION ===" << std::endl;
    std::cout << "The encodeMove() function in MoveEncoding.cpp assumes that:" << std::endl;
    std::cout << "  - Any move from e8 to g8 is black kingside castling" << std::endl;
    std::cout << "  - Any move from e8 to c8 is black queenside castling" << std::endl;
    std::cout << "But in our position, there's a WHITE ROOK on e8, not the black king!" << std::endl;
    std::cout << "So e8-g8 and e8-c8 should be normal rook moves (special=0)," << std::endl;
    std::cout << "not castling moves (special=3)." << std::endl;
    
    return 0;
}
