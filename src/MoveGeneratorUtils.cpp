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

void debugEnPassant(const Board& board, bool isWhite) {
    std::cout << "=== En Passant Mask Debug Info ===\n";
    std::cout << "En Passant Square Index: " << board.getEnPassantSquare() << "\n";
    std::cout << "En Passant Square Bitboard: " << std::hex
              << (1ULL << board.getEnPassantSquare()) << "\n";

    uint64_t whitePawns = board.getWhitePawns();
    uint64_t blackPawns = board.getBlackPawns();
    uint64_t ep = 1ULL << board.getEnPassantSquare();

    printBitboard(whitePawns, "White Pawns");
    printBitboard(blackPawns, "Black Pawns");

    uint64_t fromMask = isWhite
        ? ((ep >> 9) & whitePawns & 0xFEFEFEFEFEFEFEFEULL) |
          ((ep >> 7) & whitePawns & 0x7F7F7F7F7F7F7F7FULL)
        : ((ep << 7) & blackPawns & 0xFEFEFEFEFEFEFEFEULL) |
          ((ep << 9) & blackPawns & 0x7F7F7F7F7F7F7F7FULL);

    std::cout << "Eligible Pawns Bitboard: " << std::hex << fromMask << "\n";
    printBitboard(fromMask, "Eligible Pawns");

    if (fromMask)
        std::cout << "Valid En Passant Capture Found!\n";
    else
        std::cout << "No valid en passant capture found.\n";

    std::cout << "==================================\n";
}
