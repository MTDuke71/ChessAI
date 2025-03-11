#pragma once
#include <iostream>
#include <array>
#include <cstdint>

class Board {
private:
    uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;

public:
    Board();
    void printBoard() const;
};
