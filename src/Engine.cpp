#include "Engine.h"
#include <algorithm>
#include <array>

// Helper to remove and get index of least significant bit
static int popLSBIndex(uint64_t &bb) {
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanForward64(&index, bb);
    bb &= bb - 1;
    return static_cast<int>(index);
#else
    int index = __builtin_ctzll(bb);
    bb &= bb - 1;
    return index;
#endif
}

// Cross-platform helper to get index of least significant bit without
// modifying the input bitboard
static int lsbIndex(uint64_t bb) {
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanForward64(&index, bb);
    return static_cast<int>(index);
#else
    return __builtin_ctzll(bb);
#endif
}

// Mirror a square for black piece-square tables
static int mirror(int sq) {
    return ((7 - (sq / 8)) * 8) + (sq % 8);
}

namespace {
// Simple piece-square tables for a slightly better evaluation
const std::array<int, 64> pawnTable = {
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10,-20,-20, 10, 10,  5,
     5, -5,-10,  0,  0,-10, -5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5,  5, 10, 25, 25, 10,  5,  5,
    10, 10, 20, 30, 30, 20, 10, 10,
    50, 50, 50, 50, 50, 50, 50, 50,
     0,  0,  0,  0,  0,  0,  0,  0
};

const std::array<int, 64> knightTable = {
   -50,-40,-30,-30,-30,-30,-40,-50,
   -40,-20,  0,  0,  0,  0,-20,-40,
   -30,  0, 10, 15, 15, 10,  0,-30,
   -30,  5, 15, 20, 20, 15,  5,-30,
   -30,  0, 15, 20, 20, 15,  0,-30,
   -30,  5, 10, 15, 15, 10,  5,-30,
   -40,-20,  0,  5,  5,  0,-20,-40,
   -50,-40,-30,-30,-30,-30,-40,-50
};

const std::array<int, 64> bishopTable = {
   -20,-10,-10,-10,-10,-10,-10,-20,
   -10,  0,  0,  0,  0,  0,  0,-10,
   -10,  0,  5, 10, 10,  5,  0,-10,
   -10,  5,  5, 10, 10,  5,  5,-10,
   -10,  0, 10, 10, 10, 10,  0,-10,
   -10, 10, 10, 10, 10, 10, 10,-10,
   -10,  5,  0,  0,  0,  0,  5,-10,
   -20,-10,-10,-10,-10,-10,-10,-20
};

const std::array<int, 64> rookTable = {
     0,  0,  0,  5,  5,  0,  0,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     5, 10, 10, 10, 10, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

const std::array<int, 64> queenTable = {
   -20,-10,-10, -5, -5,-10,-10,-20,
   -10,  0,  0,  0,  0,  0,  0,-10,
   -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
     0,  0,  5,  5,  5,  5,  0, -5,
   -10,  5,  5,  5,  5,  5,  0,-10,
   -10,  0,  5,  0,  0,  0,  0,-10,
   -20,-10,-10, -5, -5,-10,-10,-20
};

const std::array<int, 64> kingTable = {
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -20,-30,-30,-40,-40,-30,-30,-20,
   -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};
}


int Engine::evaluate(const Board& b) const {
    const int pawn = 100, knight = 320, bishop = 330, rook = 500, queen = 900, king = 20000;
    int score = 0;

    uint64_t pieces;

    pieces = b.getWhitePawns();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score += pawn + pawnTable[sq];
    }
    pieces = b.getBlackPawns();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score -= pawn + pawnTable[mirror(sq)];
    }

    pieces = b.getWhiteKnights();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score += knight + knightTable[sq];
    }
    pieces = b.getBlackKnights();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score -= knight + knightTable[mirror(sq)];
    }

    pieces = b.getWhiteBishops();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score += bishop + bishopTable[sq];
    }
    pieces = b.getBlackBishops();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score -= bishop + bishopTable[mirror(sq)];
    }

    pieces = b.getWhiteRooks();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score += rook + rookTable[sq];
    }
    pieces = b.getBlackRooks();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score -= rook + rookTable[mirror(sq)];
    }

    pieces = b.getWhiteQueens();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score += queen + queenTable[sq];
    }
    pieces = b.getBlackQueens();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score -= queen + queenTable[mirror(sq)];
    }

    pieces = b.getWhiteKing();
    if (pieces) {
        int sq = lsbIndex(pieces);
        score += king + kingTable[sq];
    }
    pieces = b.getBlackKing();
    if (pieces) {
        int sq = lsbIndex(pieces);
        score -= king + kingTable[mirror(sq)];
    }

    return score;
}

int Engine::minimax(Board& board, int depth, int alpha, int beta, bool maximizing) {
    if (depth == 0) return evaluate(board);
    auto moves = generator.generateAllMoves(board, board.isWhiteToMove());
    if (moves.empty()) return evaluate(board);
    if (maximizing) {
        int maxEval = -1000000;
        for (const auto& m : moves) {
            Board copy = board;
            copy.makeMove(m);
            int eval = minimax(copy, depth - 1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = 1000000;
        for (const auto& m : moves) {
            Board copy = board;
            copy.makeMove(m);
            int eval = minimax(copy, depth - 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

std::string Engine::searchBestMove(Board& board, int depth) {
    auto moves = generator.generateAllMoves(board, board.isWhiteToMove());
    std::string bestMove;
    int bestScore = board.isWhiteToMove() ? -1000000 : 1000000;
    for (const auto& m : moves) {
        Board copy = board;
        copy.makeMove(m);
        int score = minimax(copy, depth - 1, -1000000, 1000000, !board.isWhiteToMove());
        if (board.isWhiteToMove()) {
            if (score > bestScore) { bestScore = score; bestMove = m; }
        } else {
            if (score < bestScore) { bestScore = score; bestMove = m; }
        }
    }
    return bestMove;
}
