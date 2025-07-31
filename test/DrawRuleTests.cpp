#include "Board.h"
#include "Zobrist.h"
#include <cassert>
#include <iostream>

void testFiftyMoveRule() {
    Board b;
    b.loadFEN("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
    for (int i=0;i<50;i++) {
        b.makeMove("e1-e2");
        b.makeMove("e8-e7");
        b.makeMove("e2-e1");
        b.makeMove("e7-e8");
    }
    assert(b.isFiftyMoveDraw());
    std::cout << "[✔] Fifty-move rule detected\n";
}

void testThreefoldRepetition() {
    Board b;
    b.loadFEN("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
    b.makeMove("e1-e2");
    b.makeMove("e8-e7");
    b.makeMove("e2-e1");
    b.makeMove("e7-e8");
    assert(!b.isThreefoldRepetition());
    b.makeMove("e1-e2");
    b.makeMove("e8-e7");
    b.makeMove("e2-e1");
    b.makeMove("e7-e8");
    assert(b.isThreefoldRepetition());
    std::cout << "[✔] Threefold repetition detected\n";
}

int main(){
    Zobrist::init();
    testFiftyMoveRule();
    testThreefoldRepetition();
    std::cout << "All tests done\n";
}
