#include "Board.h"
#include <iostream>

Board::Board() {
    // Initialize board with default starting positions
    whitePawns = 0x000000000000FF00;
    blackPawns = 0x00FF000000000000;

    whiteRooks = 0x0000000000000081;
    whiteKnights = 0x0000000000000042;
    whiteBishops = 0x0000000000000024;
    whiteQueens = 0x0000000000000008;
    whiteKing = 0x0000000000000010;

    blackRooks = 0x8100000000000000;
    blackKnights = 0x4200000000000000;
    blackBishops = 0x2400000000000000;
    blackQueens = 0x0800000000000000;
    blackKing = 0x1000000000000000;

    // Initialize En Passant tracking
    enPassantSquare = -1; // Default to -1 (invalid position)
}

void Board::clearBoard() {
    whitePawns = whiteKnights = whiteBishops = whiteRooks = whiteQueens = whiteKing = 0;
    blackPawns = blackKnights = blackBishops = blackRooks = blackQueens = blackKing = 0;
    enPassantSquare = -1;
}

void Board::printBoard() const {
    std::cout << "   a b c d e f g h\n";
    std::cout << "  +-----------------+\n";

    for (int rank = 7; rank >= 0; --rank) {
        std::cout << " " << rank + 1 << "|";

        for (int file = 0; file < 8; ++file) {
            uint64_t square = 1ULL << (rank * 8 + file);
            if (whitePawns & square) std::cout << " P";
            else if (whiteRooks & square) std::cout << " R";
            else if (whiteKnights & square) std::cout << " N";
            else if (whiteBishops & square) std::cout << " B";
            else if (whiteQueens & square) std::cout << " Q";
            else if (whiteKing & square) std::cout << " K";
            else if (blackPawns & square) std::cout << " p";
            else if (blackRooks & square) std::cout << " r";
            else if (blackKnights & square) std::cout << " n";
            else if (blackBishops & square) std::cout << " b";
            else if (blackQueens & square) std::cout << " q";
            else if (blackKing & square) std::cout << " k";
            else std::cout << " .";
        }
        std::cout << " | " << rank + 1 << "\n";
    }

    std::cout << "  +-----------------+\n";
    std::cout << "   a b c d e f g h\n\n";
}
