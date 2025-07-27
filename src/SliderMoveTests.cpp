#include "Board.h"
#include "MoveGenerator.h"
#include "PrintMoves.h"
#include <cassert>
#include <iostream>

void testRookMoves() {
    Board board;
    board.clearBoard();
    board.setWhiteRooks(1ULL << 27); // d4

    MoveGenerator gen;
    auto moves = gen.generateRookMoves(board, true);
    std::cout << "\n[?] Rook Moves\n";
    printMoves(moves);
    assert(moves.size() == 14);
}

void testBishopMoves() {
    Board board;
    board.clearBoard();
    board.setWhiteBishops(1ULL << 27); // d4

    MoveGenerator gen;
    auto moves = gen.generateBishopMoves(board, true);
    std::cout << "\n[?] Bishop Moves\n";
    printMoves(moves);
    assert(moves.size() == 13);
}

void testQueenMoves() {
    Board board;
    board.clearBoard();
    board.setWhiteQueens(1ULL << 27); // d4

    MoveGenerator gen;
    auto moves = gen.generateQueenMoves(board, true);
    std::cout << "\n[?] Queen Moves\n";
    printMoves(moves);
    assert(moves.size() == 27);
}

int main() {
    testRookMoves();
    testBishopMoves();
    testQueenMoves();
    std::cout << "\nAll slider move tests passed!\n";
    return 0;
}
