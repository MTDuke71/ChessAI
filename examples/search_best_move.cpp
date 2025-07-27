#include <iostream>
#include "Board.h"
#include "Engine.h"

int main() {
    Board board; // start position by default
    Engine engine;

    std::string best = engine.searchBestMove(board, 2);
    std::cout << "Best move: " << best << "\n";
    return 0;
}
