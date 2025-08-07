#include "Board.h"
#include "MoveGenerator.h"
#include "PrintMoves.h"
#include "MoveEncoding.h"
#include <cassert>
#include <iostream>

void testBasicKingMoves() {
    Board b;
    b.loadFEN("8/8/8/3K4/8/8/8/8 w - - 0 1");
    MoveGenerator g;
    std::vector<uint16_t> moves = g.generateKingMoves(b, true);
    std::cout << "\n[?] King Moves from d5" << std::endl;
    printMoves(moves);
    assert(moves.size() == 8);
}

void testCastling() {
    Board b;
    b.loadFEN("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    MoveGenerator g;
    std::vector<uint16_t> wmoves = g.generateKingMoves(b, true);
    std::cout << "\n[?] White Castling" << std::endl;
    printMoves(wmoves);
    bool hasK = false, hasQ = false;
    for (auto m : wmoves) {
        std::string s = decodeMove(m);
        if (s == "e1-g1") hasK = true;
        if (s == "e1-c1") hasQ = true;
    }
    assert(hasK && hasQ);

    std::vector<uint16_t> bmoves = g.generateKingMoves(b, false);
    bool bK = false, bQ = false;
    for (auto m : bmoves) {
        std::string s = decodeMove(m);
        if (s == "e8-g8") bK = true;
        if (s == "e8-c8") bQ = true;
    }
    assert(bK && bQ);
}

void testNoCastlingWhileInCheck() {
    Board b;
    b.clearBoard();
    b.setWhiteKing(1ULL << 4);   // e1
    b.setWhiteRooks((1ULL<<0) | (1ULL<<7));
    b.setBlackRooks(1ULL << 12); // e2 giving check
    b.setBlackKing(1ULL << 60);
    b.setCastleWK(true);
    b.setCastleWQ(true);
    MoveGenerator g;
    std::vector<uint16_t> moves = g.generateKingMoves(b, true);
    for (auto m : moves) {
        std::string s = decodeMove(m);
        assert(s != "e1-g1" && s != "e1-c1");
    }
}

void testNoCastlingThroughCheck() {
    Board b;
    b.clearBoard();
    b.setWhiteKing(1ULL << 4);
    b.setWhiteRooks((1ULL<<0) | (1ULL<<7));
    b.setBlackRooks(1ULL << 21); // f3 attacking f1
    b.setBlackKing(1ULL << 60);
    b.setCastleWK(true);
    MoveGenerator g;
    std::vector<uint16_t> moves = g.generateKingMoves(b, true);
    for (auto m : moves) {
        std::string s = decodeMove(m);
        assert(s != "e1-g1");
    }
}

void testRookMoveDisablesCastling() {
    Board b;
    b.loadFEN("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    b.makeMove("h1-h2");
    MoveGenerator g;
    std::vector<uint16_t> moves = g.generateKingMoves(b, true);
    for (auto m : moves) {
        std::string s = decodeMove(m);
        assert(s != "e1-g1");
    }
}

int main() {
    testBasicKingMoves();
    testCastling();
    testNoCastlingWhileInCheck();
    testNoCastlingThroughCheck();
    testRookMoveDisablesCastling();
    std::cout << "\nAll king move tests passed!" << std::endl;
    return 0;
}
