#pragma once
#include "Board.h"
#include "MoveGenerator.h"
#include <cstdint>

uint64_t perft(Board& board, MoveGenerator& generator, int depth);
uint64_t perft(Board& board, MoveGenerator& generator, int depth, double& ms);
uint64_t perftDivide(Board& board, MoveGenerator& generator, int depth);
