#include "Board.h"
#include <cassert>
#include <iostream>

void testCheckmate() {
    Board b;
    b.loadFEN("7k/6Q1/7K/8/8/8/8/8 b - - 0 1");
    assert(b.isCheckmate());
    std::cout << "[✔] Checkmate detected\n";
}

int main() {
    testCheckmate();
    std::cout << "All tests done\n";
    return 0;
}
