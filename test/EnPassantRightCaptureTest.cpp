#include "Board.h"
#include <cassert>
#include <iostream>

int main() {
    Board board;
    board.loadFEN("r1b1rnk1/pp2bpp1/8/4NP1n/4p1Pp/P3B2P/1PP1P3/2KR1B1R b - g3 0 18");
    assert(board.isMoveLegal("h4-g3"));
    board.makeMove("h4-g3");
    std::cout << "En passant capture executed.\n";
    return 0;
}

