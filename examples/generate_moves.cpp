#include <iostream>
#include "Board.h"
#include "MoveGenerator.h"
#include "PrintMoves.h"

int main() {
    Board board;
    MoveGenerator gen;

    // Load a custom position from FEN
    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    auto moves = gen.generateAllMoves(board, board.isWhiteToMove());
    printMoves(moves);
    return 0;
}
