#include "src/Engine.h"
#include "src/Board.h"
#include <iostream>

int main() {
    Engine engine;
    Board board;
    
    std::cout << "Starting position evaluation: " << engine.evaluate(board) << std::endl;
    
    // Test making a move
    board.makeMove(0x0c0c); // e2e4
    std::cout << "After e2e4 evaluation: " << engine.evaluate(board) << std::endl;
    
    return 0;
}
