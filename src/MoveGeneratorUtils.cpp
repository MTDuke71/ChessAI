#include "MoveGenerator.h"
#include "Board.h"
#include "BitUtils.h"
#include <iostream>
#include <string>
#include <array>

namespace {
const std::array<std::string, 64> algebraicSquares = [] {
    std::array<std::string, 64> arr{};
    for (int i = 0; i < 64; ++i) {
        char file = 'a' + (i % 8);
        char rank = '1' + (i / 8);
        arr[i] = std::string{file} + rank;
    }
    return arr;
}();
} // namespace

std::string squareToNotation(int square) {
    char file = 'a' + (square % 8);
    char rank = '1' + (square / 8);
    return std::string{file} + rank;
}

std::string indexToAlgebraic(int index) {
    return algebraicSquares[index];
}

void printBitboard(uint64_t bitboard, const std::string& label) {
    std::cout << label << ":\n";
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::cout << ((bitboard & (1ULL << square)) ? " P " : " . ");
        }
        std::cout << "\n";
    }
    std::cout << "==========================\n";
}


