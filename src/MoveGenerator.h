#pragma once
#include "Board.h"
#include <vector>
#include <cstdint>

// Encoding move data
#define MOVE(source, target) ((source) | ((target) << 6))

class MoveGenerator {
public:
    std::vector<uint32_t> generatePawnMoves(const Board& board, bool isWhite);

private:
    void addMoves(std::vector<uint32_t>& moves, uint64_t pawns, uint64_t targetSquares, int shift);
};
