#include "Board.h"
#include <cassert>
#include <iostream>

void testPawnAttackUpdate() {
    Board b;
    b.loadFEN("8/8/8/8/8/8/4P3/4K3 w - - 0 1");
    uint64_t before = b.getWhiteAttacks();
    uint64_t expectedBefore = (1ULL<<3)|(1ULL<<5)|(1ULL<<11)|(1ULL<<12)|(1ULL<<13)|(1ULL<<19)|(1ULL<<21);
    assert(before == expectedBefore);

    Board::MoveState st;
    b.makeMove("e2-e4", st);
    uint64_t after = b.getWhiteAttacks();
    uint64_t expectedAfter = (1ULL<<3)|(1ULL<<5)|(1ULL<<11)|(1ULL<<12)|(1ULL<<13)|(1ULL<<35)|(1ULL<<37);
    assert(after == expectedAfter);
    b.unmakeMove(st);
    assert(before == b.getWhiteAttacks());
    std::cout << "[✔] Incremental attack update test passed\n";
}

int main() {
    testPawnAttackUpdate();
    return 0;
}
