#include <iostream>
#include "Board.h"
#include "Engine.h"

int main() {
    Board board;
    Engine engine;
    std::string best = engine.searchBestMove(board, 2);
    std::cout << "Best move from starting position: " << best << "\n";
    return 0;
}
