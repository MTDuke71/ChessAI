#include <iostream>
#include "Board.h"
#include "MoveGenerator.h"
#include "PrintMoves.h"

int main() {
    Board board;
    MoveGenerator moveGenerator;

    std::vector<std::string> moves = moveGenerator.generatePawnMoves(board, true); // Updated to std::string
    std::cout << "Pawn Moves (Algebraic Notation):\n";
    printMoves(moves);  // Updated to call the new print function for strings

    return 0;
}