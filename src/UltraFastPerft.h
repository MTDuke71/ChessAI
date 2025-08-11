#pragma once
#include "BBCStyleEngine.h"
#include "Board.h"
#include <cstdint>

// Ultra-fast perft using BBC-style engine core with full move generation
class UltraFastPerft {
public:
    static uint64_t perft(const Board& board, int depth);
    static uint64_t perftDivide(const Board& board, int depth);
    static uint64_t perftTimed(const Board& board, int depth, double& ms);
    
private:
    // Convert our Board to BBC-style representation
    static void boardToBBC(const Board& board, BBCStyleEngine& bbcEngine);
    
    // BBC-style ultra-fast recursive perft
    static uint64_t perftRecursive(BBCStyleEngine& engine, int depth);
    
    // Full move generation in BBC style
    static void generateAllMoves(BBCStyleEngine& engine, BBCStyleEngine::MoveList& moves);
    
    // Piece-specific move generation (BBC-style)
    static void generatePawnMoves(BBCStyleEngine& engine, BBCStyleEngine::MoveList& moves);
    static void generateKnightMoves(BBCStyleEngine& engine, BBCStyleEngine::MoveList& moves);
    static void generateBishopMoves(BBCStyleEngine& engine, BBCStyleEngine::MoveList& moves);
    static void generateRookMoves(BBCStyleEngine& engine, BBCStyleEngine::MoveList& moves);
    static void generateQueenMoves(BBCStyleEngine& engine, BBCStyleEngine::MoveList& moves);
    static void generateKingMoves(BBCStyleEngine& engine, BBCStyleEngine::MoveList& moves);
};
