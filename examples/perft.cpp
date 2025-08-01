#include <iostream>
#include "Board.h"
#include "MoveGenerator.h"
#include "Perft.h"

int main(int argc, char* argv[]) {
    Board board;
    MoveGenerator gen;

    if (argc > 1) {
        if (!board.loadFEN(argv[1])) {
            std::cerr << "Failed to load FEN" << std::endl;
            return 1;
        }
    }

    int depth = 1;
    if (argc > 2) {
        depth = std::stoi(argv[2]);
    }

    double ms = 0.0;
    uint64_t nodes = perft(board, gen, depth, ms);
    std::cout << "Perft(" << depth << ") = " << nodes << " in " << ms << " ms\n";
    return 0;
}
