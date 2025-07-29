#pragma once
#include "Board.h"
#include "MoveGenerator.h"
#include "TranspositionTable.h"
#include "Zobrist.h"
#include <string>
#include <chrono>
#include <atomic>
#include <utility>
#include <unordered_map>

class Engine {
public:
    Engine();
    enum class GamePhase { Opening, Middlegame, Endgame };

    GamePhase getGamePhase(const Board& board) const;
    int evaluate(const Board& board) const;
    std::pair<int, std::string>
    minimax(Board& board, int depth, int alpha, int beta, bool maximizing,
            const std::chrono::steady_clock::time_point& end,
            const std::atomic<bool>& stop);

    std::string searchBestMove(Board& board, int depth);

    std::string searchBestMoveTimed(Board& board, int maxDepth,
                                    int timeLimitMs,
                                    std::atomic<bool>& stopFlag);

private:
    MoveGenerator generator;
    uint64_t nodes = 0;
    TranspositionTable tt;
};
