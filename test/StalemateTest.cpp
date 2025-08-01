#include "Board.h"
#include <cassert>
#include <iostream>

void testStalemate() {
    Board b;
    b.loadFEN("7k/5Q2/6K1/8/8/8/8/8 b - - 0 1");
    assert(b.isStalemate());
    std::cout << "[✔] Stalemate detected\n";
}

int main() {
    testStalemate();
    std::cout << "All tests done\n";
    return 0;
}
