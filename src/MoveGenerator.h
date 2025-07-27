#pragma once
#include "Board.h"
#include <vector>
#include <cstdint>
#include <string>

// Encoding move data
#define MOVE(source, target) ((source) | ((target) << 6))

class MoveGenerator {
public:
    std::vector<std::string> generatePawnMoves(const Board& board, bool isWhite);
    std::vector<std::string> generateKnightMoves(const Board& board, bool isWhite);
    std::vector<std::string> generateRookMoves(const Board& board, bool isWhite);
    std::vector<std::string> generateBishopMoves(const Board& board, bool isWhite);
    std::vector<std::string> generateQueenMoves(const Board& board, bool isWhite);
    void addMoves(std::vector<std::string>& moves, uint64_t pawns, uint64_t moveBoard, int shift);
};

std::string indexToAlgebraic(int index);
