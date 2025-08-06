#include "Perft.h"
#include "Board.h"
#include "MoveGenerator.h"
#include <cassert>
#include <iostream>

int main() {
    Board board;
    MoveGenerator gen;
    bool loaded = board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    assert(loaded);

    uint64_t total = perft(board, gen, 2);
    uint64_t divided = perftDivide(board, gen, 2);
    assert(total == divided);
    std::cout << "Perft divide total = " << divided << "\n";
    return 0;
}
