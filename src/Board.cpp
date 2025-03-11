#include "Board.h"

Board::Board() {
    whitePawns = 0x00FF000000000000;
    whiteRooks = 0x8100000000000000;
    whiteKnights = 0x4200000000000000;
    whiteBishops = 0x2400000000000000;
    whiteQueens = 0x0800000000000000;
    whiteKing = 0x1000000000000000;

    blackPawns = 0x000000000000FF00;
    blackRooks = 0x0000000000000081;
    blackKnights = 0x0000000000000042;
    blackBishops = 0x0000000000000024;
    blackQueens = 0x0000000000000008;
    blackKing = 0x0000000000000010;
}

void Board::printBoard() const {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            uint64_t square = 1ULL << (rank * 8 + file);
            if (whitePawns & square) std::cout << "P ";
            else if (whiteRooks & square) std::cout << "R ";
            else if (whiteKnights & square) std::cout << "N ";
            else if (whiteBishops & square) std::cout << "B ";
            else if (whiteQueens & square) std::cout << "Q ";
            else if (whiteKing & square) std::cout << "K ";
            else if (blackPawns & square) std::cout << "p ";
            else if (blackRooks & square) std::cout << "r ";
            else if (blackKnights & square) std::cout << "n ";
            else if (blackBishops & square) std::cout << "b ";
            else if (blackQueens & square) std::cout << "q ";
            else if (blackKing & square) std::cout << "k ";
            else std::cout << ". ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}
