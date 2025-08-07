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

void testCheckDetection() {
    Board board;
    board.loadFEN("k3Q2r/p1p3p1/6p1/2p2b2/2pr1P1p/P3R2P/1PP2B2/2K3R1 b - - 0 33");
    // Black is in check from the white queen on e8. Capturing on f4 does not resolve it.
    assert(!board.isMoveLegal("d4-f4"));
    // Capturing the checking queen is legal.
    assert(board.isMoveLegal("h8-e8"));
}

void testKingIntoCheck() {
    Board board;
    board.loadFEN("5Q2/2p3p1/2p3p1/8/k4PRp/4r2P/1pP5/1K6 b - - 0 40");
    // Moving the king to a3 would step into a queen attack.
    assert(!board.isMoveLegal("a4-a3"));
    // Capturing on h3 remains legal.
    assert(board.isMoveLegal("e3-h3"));
}

void testRookCaptureNotCastling() {
    Board board;
    board.loadFEN("r7/1p3p1p/2p2k2/3p4/8/8/PPP2P1P/2KBr1R1 b - - 10 36");
    // Ensure a rook move from e1 to g1 is recognized as a normal capture, not castling.
    assert(board.isMoveLegal("e1-g1"));
}

int main() {
    testIllegalMove();
    testLegalMove();
    testCheckDetection();
    testKingIntoCheck();
    testRookCaptureNotCastling();
    std::cout << "\nIllegal move tests passed!\n";
    return 0;
}
