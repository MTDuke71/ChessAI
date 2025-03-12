#include <iostream>
#include "PrintMoves.h"

void printMoves(const std::vector<std::string>& moves) {
    for (const auto& move : moves) {
        std::cout << "Move: " << move << "\n";
    }
}