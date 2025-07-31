#include "Board.h"
#include "MoveGenerator.h"
#include "PrintMoves.h"
#include <cassert>
#include <iostream>

void testPinnedRook() {
    Board board;
    board.clearBoard();
    board.setWhiteKing(1ULL << 2);      // c1
    board.setWhiteRooks(1ULL << 1);     // b1
    board.setBlackRooks(1ULL << 0);     // a1
    board.setBlackKing(1ULL << 63);     // h8 (arbitrary)

    MoveGenerator gen;
    auto pseudo = gen.generateAllMoves(board, true);
    auto legal = gen.generateLegalMoves(board, true);


    bool hasIllegal = false;
    for (const auto& m : pseudo) {
        if (m.rfind("b1-", 0) == 0 && m != "b1-a1") {
            hasIllegal = true;
            break;
        }
    }
    assert(hasIllegal);

    for (const auto& m : legal) {
        if (m.rfind("b1-", 0) == 0 && m != "b1-a1") {
            assert(false && "illegal move returned by generateLegalMoves");
        }
    }
}

int main() {
    testPinnedRook();
    std::cout << "\nLegal move generation test passed!\n";
    return 0;
}
