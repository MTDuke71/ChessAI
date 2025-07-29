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
    std::vector<std::string> generatePawnMoves(const Board& board, bool isWhite) const;
    std::vector<std::string> generateKnightMoves(const Board& board, bool isWhite) const;
    std::vector<std::string> generateRookMoves(const Board& board, bool isWhite) const;
    std::vector<std::string> generateBishopMoves(const Board& board, bool isWhite) const;
    std::vector<std::string> generateQueenMoves(const Board& board, bool isWhite) const;
    std::vector<std::string> generateKingMoves(const Board& board, bool isWhite) const;
    std::vector<std::string> generateAllMoves(const Board& board, bool isWhite) const;
    void addMoves(std::vector<std::string>& moves, uint64_t pawns, uint64_t moveBoard, int shift) const;

    bool isSquareAttacked(const Board& board, int square, bool byWhite) const;
    bool isKingInCheck(const Board& board, bool white) const;
};

std::string indexToAlgebraic(int index);
