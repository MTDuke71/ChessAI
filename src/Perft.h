#pragma once
#include "Board.h"
#include "MoveGenerator.h"
#include <cstdint>

uint64_t perft(Board& board, MoveGenerator& generator, int depth);
