#include "Board.h"
#include <iostream>
#include <sstream>
#include <cctype>

Board::Board() {
    loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void Board::clearBoard() {
    whitePawns = whiteKnights = whiteBishops = whiteRooks = whiteQueens = whiteKing = 0;
    blackPawns = blackKnights = blackBishops = blackRooks = blackQueens = blackKing = 0;
    enPassantSquare = -1;
    whiteToMove = true;
    castleWK = castleWQ = castleBK = castleBQ = false;
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

bool Board::loadFEN(const std::string& fen) {
    clearBoard();
    std::stringstream ss(fen);
    std::string boardPart, active, castling, ep;

    if (!(ss >> boardPart >> active >> castling >> ep))
        return false;

    int rank = 7, file = 0;
    for (char c : boardPart) {
        if (c == '/') {
            --rank; file = 0; continue;
        }
        if (std::isdigit(c)) { file += c - '0'; continue; }

        int index = rank * 8 + file;
        uint64_t bit = 1ULL << index;
        switch (c) {
            case 'P': whitePawns |= bit; break;
            case 'N': whiteKnights |= bit; break;
            case 'B': whiteBishops |= bit; break;
            case 'R': whiteRooks |= bit; break;
            case 'Q': whiteQueens |= bit; break;
            case 'K': whiteKing |= bit; break;
            case 'p': blackPawns |= bit; break;
            case 'n': blackKnights |= bit; break;
            case 'b': blackBishops |= bit; break;
            case 'r': blackRooks |= bit; break;
            case 'q': blackQueens |= bit; break;
            case 'k': blackKing |= bit; break;
        }
        ++file;
    }

    whiteToMove = (active == "w");
    castleWK = castling.find('K') != std::string::npos;
    castleWQ = castling.find('Q') != std::string::npos;
    castleBK = castling.find('k') != std::string::npos;
    castleBQ = castling.find('q') != std::string::npos;

    if (ep != "-") {
        int f = ep[0] - 'a';
        int r = ep[1] - '1';
        enPassantSquare = r * 8 + f;
    } else {
        enPassantSquare = -1;
    }

    return true;
}
