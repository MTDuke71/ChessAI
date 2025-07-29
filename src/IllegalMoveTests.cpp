#include "Board.h"
#include <cassert>
#include <iostream>

void testIllegalPawnLeap() {
    Board board; // start position
    bool ok = board.makeMove("e2-e5");
    assert(!ok); // pawn cannot move three squares
}

void testMoveFromEmptySquare() {
    Board board; // start position
    bool ok = board.makeMove("e3-e4");
    assert(!ok);
}

void testWrongSidePiece() {
    Board board; // start position white to move
    bool ok = board.makeMove("a7-a6");
    assert(!ok); // black pawn cannot move when white to move
}

void testLegalMove() {
    Board board;
    bool ok = board.makeMove("e2-e4");
    assert(ok);
    // After move, e4 should contain a white pawn
    int from = algebraicToIndex("e4");
    assert(board.getWhitePawns() & (1ULL << from));
}

int main() {
    testIllegalPawnLeap();
    testMoveFromEmptySquare();
    testWrongSidePiece();
    testLegalMove();
    std::cout << "\nIllegal move tests passed!" << std::endl;
    return 0;
}
