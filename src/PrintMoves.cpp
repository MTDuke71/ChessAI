// -----------------------------------------------------------------------------
// Utility to print a list of moves to the console.
// -----------------------------------------------------------------------------
#include <iostream>
#include "PrintMoves.h"

// -----------------------------------------------------------------------------
// Prints each move in the provided list on its own line.
// -----------------------------------------------------------------------------
void printMoves(const std::vector<std::string>& moves) {
    for (const auto& move : moves) {
        std::cout << "Move: " << move << "\n";
    }
}
