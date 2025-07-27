#include <iostream>
#include "Board.h"

int main() {
    Board board;
    board.clearBoard();
    // Place white king on e1 and black king on e8
    board.setWhiteKing(1ULL << algebraicToIndex("e1"));
    board.setBlackKing(1ULL << algebraicToIndex("e8"));
    // Add a white pawn on e2
    board.setWhitePawns(1ULL << algebraicToIndex("e2"));

    board.printBoard();
    return 0;
}
