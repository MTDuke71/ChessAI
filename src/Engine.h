#pragma once
#include "Board.h"
#include "BBCStyleEngine.h"
#include "TranspositionTable.h"
#include "Zobrist.h"
#include "OpeningBook.h"
#include "Tablebase.h"
#include "ThreadPool.h"
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
            const std::atomic<bool>& stop, int ply = 0);

    // Simple negamax search with alpha-beta pruning
    int negamaxAlphaBeta(Board& board, int depth,
                         int alpha, int beta, int color,
                         const std::chrono::steady_clock::time_point& end,
                         const std::atomic<bool>& stop);

    std::string searchBestMove(Board& board, int depth);

    std::string searchBestMoveTimed(Board& board, int maxDepth,
                                    int timeLimitMs,
                                    std::atomic<bool>& stopFlag);

    void clearTranspositionTable() { tt.clear(); }
    void setHashSizeMB(size_t mb);
    size_t getHashSize() const { return tt.size(); }
    void setOwnBook(bool enabled) { useOwnBook = enabled; }
    bool isOwnBookEnabled() const { return useOwnBook; }

private:
    int quiescence(Board& board, int alpha, int beta, bool maximizing,
                   const std::chrono::steady_clock::time_point& end,
                   const std::atomic<bool>& stop);
    BBCStyleEngine bbcEngine;  // Ultra-fast BBC-style engine
    std::atomic<uint64_t> nodes = 0;
    TranspositionTable tt;
    OpeningBook book;
    Tablebase tablebase;
    ThreadPool pool;
    bool useOwnBook = false;
};
