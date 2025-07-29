#include "Board.h"
#include "Engine.h"
#include <iostream>
#include <cassert>

void testOpeningPhase() {
    Board b; // default starting position
    Engine e;
    assert(e.getGamePhase(b) == Engine::GamePhase::Opening);
    std::cout << "[✔] Opening phase detected\n";
}

void testMiddlegamePhase() {
    Board b;
    b.clearBoard();
    // Kings, rooks, knights and some pawns
    b.setWhiteKing(1ULL << 4);   // e1
    b.setBlackKing(1ULL << 60);  // e8
    b.setWhiteRooks((1ULL<<0) | (1ULL<<7));
    b.setBlackRooks((1ULL<<56) | (1ULL<<63));
    b.setWhiteKnights((1ULL<<1) | (1ULL<<6));
    b.setBlackKnights((1ULL<<57) | (1ULL<<62));
    b.setWhitePawns((1ULL<<12) | (1ULL<<13) | (1ULL<<14) | (1ULL<<15));
    b.setBlackPawns((1ULL<<52) | (1ULL<<53) | (1ULL<<54) | (1ULL<<55));
    Engine e;
    assert(e.getGamePhase(b) == Engine::GamePhase::Middlegame);
    std::cout << "[✔] Middlegame phase detected\n";
}

void testEndgamePhase() {
    Board b;
    b.clearBoard();
    b.setWhiteKing(1ULL<<4);
    b.setBlackKing(1ULL<<60);
    b.setWhitePawns(1ULL<<12);
    b.setBlackPawns(1ULL<<52);
    Engine e;
    assert(e.getGamePhase(b) == Engine::GamePhase::Endgame);
    std::cout << "[✔] Endgame phase detected\n";
}

int main() {
    testOpeningPhase();
    testMiddlegamePhase();
    testEndgamePhase();
    std::cout << "\nGame phase tests passed!" << std::endl;
    return 0;
}
