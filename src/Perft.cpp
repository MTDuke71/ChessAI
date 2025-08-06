#include "Perft.h"
#include <chrono>
#include <iostream>

uint64_t perft(Board& board, MoveGenerator& generator, int depth) {
    if (depth == 0) return 1ULL;

    auto moves = generator.generateAllMoves(board, board.isWhiteToMove());

    if (depth == 1) {
        uint64_t count = 0ULL;
        for (const auto& m : moves) {
            Board::MoveState st;
            board.makeMove(m, st);
            if (!generator.isKingInCheck(board, !board.isWhiteToMove()))
                ++count;
            board.unmakeMove(st);
        }
        return count;
    }

    uint64_t nodes = 0ULL;
    for (const auto& m : moves) {
        Board::MoveState st;
        board.makeMove(m, st);
        if (!generator.isKingInCheck(board, !board.isWhiteToMove()))
            nodes += perft(board, generator, depth - 1);
        board.unmakeMove(st);
    }
    return nodes;
}

uint64_t perft(Board& board, MoveGenerator& generator, int depth, double& ms) {
    auto start = std::chrono::steady_clock::now();
    uint64_t nodes = perft(board, generator, depth);
    auto end = std::chrono::steady_clock::now();
    ms = std::chrono::duration<double, std::milli>(end - start).count();
    return nodes;
}

uint64_t perftDivide(Board& board, MoveGenerator& generator, int depth) {
    auto moves = generator.generateAllMoves(board, board.isWhiteToMove());
    uint64_t total = 0ULL;

    for (const auto& m : moves) {
        Board::MoveState st;
        board.makeMove(m, st);
        uint64_t nodes = 0ULL;
        if (!generator.isKingInCheck(board, !board.isWhiteToMove())) {
            nodes = perft(board, generator, depth - 1);
            total += nodes;
            std::cout << m << ": " << nodes << "\n";
        }
        board.unmakeMove(st);
    }

    return total;
}
