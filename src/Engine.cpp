#include "Engine.h"
#include <algorithm>

static int popcount(uint64_t x) {
#if defined(_MSC_VER)
    return static_cast<int>(__popcnt64(x));
#else
    return __builtin_popcountll(x);
#endif
}

int Engine::evaluate(const Board& b) const {
    const int pawn = 100, knight = 320, bishop = 330, rook = 500, queen = 900, king = 20000;
    int score = 0;
    score += popcount(b.getWhitePawns()) * pawn;
    score -= popcount(b.getBlackPawns()) * pawn;
    score += popcount(b.getWhiteKnights()) * knight;
    score -= popcount(b.getBlackKnights()) * knight;
    score += popcount(b.getWhiteBishops()) * bishop;
    score -= popcount(b.getBlackBishops()) * bishop;
    score += popcount(b.getWhiteRooks()) * rook;
    score -= popcount(b.getBlackRooks()) * rook;
    score += popcount(b.getWhiteQueens()) * queen;
    score -= popcount(b.getBlackQueens()) * queen;
    score += popcount(b.getWhiteKing()) * king;
    score -= popcount(b.getBlackKing()) * king;
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
