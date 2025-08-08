#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include <iostream>

uint64_t perft(Board& board, int depth) {
    if (depth == 0) return 1;
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, board.isWhiteToMove());
    
    uint64_t nodes = 0;
    for (uint16_t move : moves) {
        Board::MoveState state;
        board.makeMove(move, state);
        nodes += perft(board, depth - 1);
        board.unmakeMove(state);
    }
    return nodes;
}

int main() {
    std::string fen = "4k3/8/8/8/8/8/8/4K2R w K - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "Position 3 perft depth 6: " << perft(board, 6) << std::endl;
    
    return 0;
}
