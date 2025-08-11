#pragma once
#include "Board.h"
#include "FastMoveGenerator.h"
#include <cstdint>

// BBC-style ultra-fast perft using FastMoveGenerator and IncrementalBoard
// This bypasses all the expensive string conversions and separate legality checks
class FastPerft {
public:
    static uint64_t perft(const Board& board, int depth);
    static uint64_t perftDivide(const Board& board, int depth);
    static uint64_t perftTimed(const Board& board, int depth, double& ms);
    
private:
    static uint64_t perftRecursive(FastMoveGenerator& gen, IncrementalBoard& board, int depth);
};
