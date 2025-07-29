#include "Board.h"
#include <cassert>
#include <iostream>

void testIllegalMove() {
    Board board;
    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    assert(!board.isMoveLegal("e2-e5"));
}

void testLegalMove() {
    Board board;
    assert(board.isMoveLegal("e2-e4"));
}

int main() {
    testIllegalMove();
    testLegalMove();
    std::cout << "\nIllegal move tests passed!\n";
    return 0;
}
