#include "Board.h"
#include "MoveGenerator.h"
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

int algebraicToIndex(const std::string& sq) {
    if (sq.size() < 2) return -1;
    int file = sq[0] - 'a';
    int rank = sq[1] - '1';
    return rank * 8 + file;
}

bool Board::isMoveLegal(const std::string& move) const {
    if (move.size() < 5) return false;
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(*this, whiteToMove);
    std::string check = move.substr(0,5);
    for (const auto& m : moves) {
        if (m.substr(0,5) == check) {
            Board copy = *this;
            copy.applyMove(move);
            return !gen.isKingInCheck(copy, !copy.isWhiteToMove());
        }
    }
    return false;
}

void Board::makeMove(const std::string& move) {
    if (!isMoveLegal(move)) {
        std::cerr << "Illegal move attempted: " << move << "\n";
        return;
    }
    applyMove(move);
}

void Board::applyMove(const std::string& move) {
    auto dash = move.find('-');
    if (dash == std::string::npos) return;
    int from = algebraicToIndex(move.substr(0, 2));
    int to = algebraicToIndex(move.substr(dash + 1, 2));
    if (from < 0 || to < 0) return;
    uint64_t fromMask = 1ULL << from;
    uint64_t toMask = 1ULL << to;

    if (toMask & whiteRooks) {
        if (to == 0) castleWQ = false;
        if (to == 7) castleWK = false;
    }
    if (toMask & blackRooks) {
        if (to == 56) castleBQ = false;
        if (to == 63) castleBK = false;
    }

    uint64_t mask = ~toMask;
    whitePawns &= mask; whiteKnights &= mask; whiteBishops &= mask; whiteRooks &= mask; whiteQueens &= mask; whiteKing &= mask;
    blackPawns &= mask; blackKnights &= mask; blackBishops &= mask; blackRooks &= mask; blackQueens &= mask; blackKing &= mask;

    auto movePiece = [&](uint64_t &bb) { if (bb & fromMask) { bb &= ~fromMask; bb |= toMask; return true; } return false; };

    bool movedWhiteKing = (whiteKing & fromMask);
    bool movedBlackKing = (blackKing & fromMask);
    bool movedWhiteRook = (whiteRooks & fromMask);
    bool movedBlackRook = (blackRooks & fromMask);

    if (!(movePiece(whitePawns) || movePiece(whiteKnights) || movePiece(whiteBishops) ||
          movePiece(whiteRooks) || movePiece(whiteQueens) || movePiece(whiteKing) ||
          movePiece(blackPawns) || movePiece(blackKnights) || movePiece(blackBishops) ||
          movePiece(blackRooks) || movePiece(blackQueens) || movePiece(blackKing))) {
        return;
    }

    if (movedWhiteKing) {
        castleWK = castleWQ = false;
        if (from == 4 && to == 6) {
            whiteRooks &= ~(1ULL<<7);
            whiteRooks |= (1ULL<<5);
        } else if (from == 4 && to == 2) {
            whiteRooks &= ~(1ULL<<0);
            whiteRooks |= (1ULL<<3);
        }
    }
    if (movedBlackKing) {
        castleBK = castleBQ = false;
        if (from == 60 && to == 62) {
            blackRooks &= ~(1ULL<<63);
            blackRooks |= (1ULL<<61);
        } else if (from == 60 && to == 58) {
            blackRooks &= ~(1ULL<<56);
            blackRooks |= (1ULL<<59);
        }
    }

    if (movedWhiteRook) {
        if (from == 0) castleWQ = false;
        if (from == 7) castleWK = false;
    }
    if (movedBlackRook) {
        if (from == 56) castleBQ = false;
        if (from == 63) castleBK = false;
    }

    whiteToMove = !whiteToMove;
}
