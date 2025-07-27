#include "Perft.h"

uint64_t perft(Board& board, MoveGenerator& generator, int depth) {
    if (depth == 0) return 1ULL;
    auto moves = generator.generateAllMoves(board, board.isWhiteToMove());
    if (depth == 1) return static_cast<uint64_t>(moves.size());

    uint64_t nodes = 0ULL;
    for (const auto& m : moves) {
        Board copy = board; // copy board for move
        copy.makeMove(m);
        nodes += perft(copy, generator, depth - 1);
    }
    return nodes;
}
