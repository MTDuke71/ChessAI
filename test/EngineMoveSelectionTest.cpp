#include "Board.h"
#include "Engine.h"
#include <cassert>
#include <iostream>

void testNoIllegalKa3() {
    Engine engine;
    Board board;
    board.loadFEN("5Q2/2p3p1/2p3p1/8/k4PRp/4r2P/1pP5/1K6 b - - 0 40");
    std::string best = engine.searchBestMove(board, 2);
    assert(best != "a4-a3");
    std::cout << "Best move: " << best << "\n";
}

int main() {
    testNoIllegalKa3();
    std::cout << "\nEngine move selection test passed!\n";
    return 0;
}
