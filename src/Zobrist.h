#pragma once
#include <cstdint>
#include "Board.h"
#include <array>

namespace Zobrist {
    extern std::array<std::array<uint64_t,64>,12> pieceHashes;
    extern uint64_t sideHash;
    extern std::array<uint64_t,4> castleHash;
    extern std::array<uint64_t,8> enPassantHash;

    void init();
    uint64_t hashBoard(const Board& b);
}

