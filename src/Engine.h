#pragma once
#include "Board.h"
#include "MoveGenerator.h"
#include <string>
#include <chrono>
#include <atomic>

class Engine {
public:
    int evaluate(const Board& board) const;
    int minimax(Board& board, int depth, int alpha, int beta, bool maximizing,
                const std::chrono::steady_clock::time_point& end,
                const std::atomic<bool>& stop);

    std::string searchBestMove(Board& board, int depth);

    std::string searchBestMoveTimed(Board& board, int maxDepth,
                                    int timeLimitMs,
                                    std::atomic<bool>& stopFlag);

private:
    MoveGenerator generator;
    uint64_t nodes = 0;
};
