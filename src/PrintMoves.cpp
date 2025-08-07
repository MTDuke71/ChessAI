// -----------------------------------------------------------------------------
// Utility to print a list of moves to the console.
// -----------------------------------------------------------------------------
#include <iostream>
#include "PrintMoves.h"
#include "MoveEncoding.h"

// -----------------------------------------------------------------------------
// Prints each move in the provided list on its own line.
// -----------------------------------------------------------------------------
void printMoves(const std::vector<uint16_t>& moves) {
    for (auto move : moves) {
        std::cout << "Move: " << decodeMove(move) << "\n";
    }
}
