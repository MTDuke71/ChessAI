#pragma once
#include "Board.h"
#include <vector>
#include <cstdint>
#include <string>

// Encoding move data
#define MOVE(source, target) ((source) | ((target) << 6))

class MoveGenerator {
public:
    MoveGenerator();
    std::vector<uint16_t> generatePawnMoves(const Board& board, bool isWhite) const;
    std::vector<uint16_t> generateKnightMoves(const Board& board, bool isWhite) const;
    std::vector<uint16_t> generateRookMoves(const Board& board, bool isWhite) const;
    std::vector<uint16_t> generateBishopMoves(const Board& board, bool isWhite) const;
    std::vector<uint16_t> generateQueenMoves(const Board& board, bool isWhite) const;
    std::vector<uint16_t> generateKingMoves(const Board& board, bool isWhite) const;
    std::vector<uint16_t> generateAllMoves(const Board& board, bool isWhite) const;
    std::vector<uint16_t> generateLegalMoves(const Board& board, bool isWhite) const;
    void addMoves(std::vector<uint16_t>& moves, uint64_t pawns, uint64_t moveBoard, int shift) const;

    bool isSquareAttacked(const Board& board, int square, bool byWhite) const;
    bool isKingInCheck(const Board& board, bool white) const;
};

std::string indexToAlgebraic(int index);
