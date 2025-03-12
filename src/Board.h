#pragma once
#include <cstdint>

class Board {
private:
    uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;

public:
    Board();

    // Getters for testing and internal logic
    uint64_t getWhitePawns() const { return whitePawns; }
    uint64_t getBlackPawns() const { return blackPawns; }
    uint64_t getWhitePieces() const { return whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing; }
    uint64_t getBlackPieces() const { return blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing; }

    // Setters for testing
    void setWhitePawns(uint64_t value) { whitePawns = value; }
    void setBlackPawns(uint64_t value) { blackPawns = value; }
    void setWhiteKing(uint64_t value) { whiteKing = value; }
    void setBlackKing(uint64_t value) { blackKing = value; }
    void setWhiteRooks(uint64_t value) { whiteRooks = value; }  // ADDED
    void setBlackQueens(uint64_t value) { blackQueens = value; }  // ADDED


    void clearBoard();  // Utility function to reset the board state
    void printBoard() const;
};
