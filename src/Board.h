#pragma once
#include <cstdint>

class Board {
private:
    uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;
    int enPassantSquare;


public:
    Board();

    // Getters for testing and internal logic
    uint64_t getWhitePawns() const { return whitePawns; }
    uint64_t getBlackPawns() const { return blackPawns; }
    uint64_t getWhitePieces() const { return whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing; }
    uint64_t getBlackPieces() const { return blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing; }
    int getEnPassantSquare() const { return enPassantSquare; }

    uint64_t getWhiteKnights() const { return whiteKnights; }
    uint64_t getBlackKnights() const { return blackKnights; }

    // Setters for testing
    void setWhitePawns(uint64_t value) { whitePawns = value; }
    void setBlackPawns(uint64_t value) { blackPawns = value; }
    void setWhiteKing(uint64_t value) { whiteKing = value; }
    void setBlackKing(uint64_t value) { blackKing = value; }
    void setWhiteRooks(uint64_t value) { whiteRooks = value; }
    void setBlackQueens(uint64_t value) { blackQueens = value; }
    void setWhiteKnights(uint64_t value) { whiteKnights = value; }
    void setBlackKnights(uint64_t value) { blackKnights = value; }
    void setEnPassantSquare(int square) { enPassantSquare = square; }

    void clearBoard();  // Utility function to reset the board state
    void printBoard() const;
};
