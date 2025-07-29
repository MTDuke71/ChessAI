#include "Board.h"
#include "MoveGenerator.h"
#include "PrintMoves.h"
#include <cassert>
#include <iostream>

void testBasicKingMoves() {
    Board b;
    b.loadFEN("8/8/8/3K4/8/8/8/8 w - - 0 1");
    MoveGenerator g;
    auto moves = g.generateKingMoves(b, true);
    std::cout << "\n[?] King Moves from d5" << std::endl;
    printMoves(moves);
    assert(moves.size() == 8);
}

void testCastling() {
    Board b;
    b.loadFEN("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    MoveGenerator g;
    auto wmoves = g.generateKingMoves(b, true);
    std::cout << "\n[?] White Castling" << std::endl;
    printMoves(wmoves);
    bool hasK = false, hasQ = false;
    for (auto &m : wmoves) {
        if (m.find("Castle Kingside") != std::string::npos) hasK = true;
        if (m.find("Castle Queenside") != std::string::npos) hasQ = true;
    }
    assert(hasK && hasQ);

    auto bmoves = g.generateKingMoves(b, false);
    bool bK = false, bQ = false;
    for (auto &m : bmoves) {
        if (m.find("Castle Kingside") != std::string::npos) bK = true;
        if (m.find("Castle Queenside") != std::string::npos) bQ = true;
    }
    assert(bK && bQ);
}

int main() {
    testBasicKingMoves();
    testCastling();
    std::cout << "\nAll king move tests passed!" << std::endl;
    return 0;
}
