#include <iostream>
#include "Board.h"
#include "MoveGenerator.h"
#include "PrintMoves.h"

int main(int argc, char* argv[]) {
    Board board;
    MoveGenerator gen;

    if (argc > 1) {
        if (!board.loadFEN(argv[1])) {
            std::cerr << "Failed to load FEN" << std::endl;
            return 1;
        }
    } else {
        board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }

    auto moves = gen.generateAllMoves(board, board.isWhiteToMove());
    printMoves(moves);
    return 0;
}
