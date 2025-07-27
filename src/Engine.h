#pragma once
#include "Board.h"
#include "MoveGenerator.h"
#include <string>

class Engine {
public:
    int evaluate(const Board& board) const;
    int minimax(Board& board, int depth, int alpha, int beta, bool maximizing);
    std::string searchBestMove(Board& board, int depth);
private:
    MoveGenerator generator;
};
