#include "Board.h"
#include "MoveGenerator.h"
#include "PrintMoves.h"
#include <cassert>
#include <iostream>

void testCenterKnightMoves() {
    Board board;
    board.clearBoard();
    board.setWhiteKnights(1ULL << 27); // Knight on d4

    MoveGenerator gen;
    std::vector<uint16_t> moves = gen.generateKnightMoves(board, true);
    std::cout << "\n[?] Center Knight Moves\n";
    printMoves(moves);
    assert(moves.size() == 8);
}

void testCornerKnightMoves() {
    Board board;
    board.clearBoard();
    board.setWhiteKnights(1ULL << 0); // Knight on a1

    MoveGenerator gen;
    std::vector<uint16_t> moves = gen.generateKnightMoves(board, true);
    std::cout << "\n[?] Corner Knight Moves\n";
    printMoves(moves);
    assert(moves.size() == 2);
}

int main() {
    testCenterKnightMoves();
    testCornerKnightMoves();
    std::cout << "\nAll knight move tests passed!\n";
    return 0;
}
