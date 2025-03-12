#include "MoveGenerator.h"
#include <vector>
#include <intrin.h>  // For MSVC's _BitScanForward64
#include <iostream>  // For printing moves

// Cross-platform bit scan (find least significant bit index)
inline int popLSBIndex(uint64_t& bitboard) {
    unsigned long index;
    _BitScanForward64(&index, bitboard);
    bitboard &= bitboard - 1;
    return static_cast<int>(index);
}

// Converts square index (0-63) to chess notation (e.g., 8 -> "e2")
std::string squareToNotation(int square) {
    char file = 'a' + (square % 8);
    char rank = '1' + (square / 8);
    return std::string{ file } + rank;
}

std::vector<uint32_t> MoveGenerator::generatePawnMoves(const Board& board, bool isWhite) {
    std::vector<uint32_t> moves;
    uint64_t pawns = isWhite ? board.getWhitePawns() : board.getBlackPawns();
    uint64_t allPieces = board.getWhitePieces() | board.getBlackPieces();
    uint64_t emptySquares = ~allPieces;

    std::cout << "Empty Squares Bitboard: " << std::hex << emptySquares << "\n";

    if (isWhite) {
        uint64_t singlePush = (pawns << 8) & emptySquares;
        addMoves(moves, pawns, singlePush, 8);

        uint64_t rank2Mask = 0x000000000000FF00;
        uint64_t doublePush = ((pawns & rank2Mask) << 16) & emptySquares & (emptySquares << 8);
        addMoves(moves, pawns, doublePush, 16);
    }
    else {
        uint64_t singlePush = (pawns >> 8) & emptySquares;
        addMoves(moves, pawns, singlePush, -8);

        uint64_t rank7Mask = 0x00FF000000000000;
        uint64_t doublePush = ((pawns & rank7Mask) >> 16) & emptySquares & (emptySquares >> 8);
        addMoves(moves, pawns, doublePush, -16);
    }

    // Display moves in chess notation (e.g., e2-e4)
    std::cout << "Pawn Moves: " << std::endl;
    for (auto move : moves) {
        int from = move & 0x3F;
        int to = (move >> 6) & 0x3F;
        std::cout << squareToNotation(from) << "-" << squareToNotation(to) << std::endl;
    }

    return moves;
}

// Corrected Placement for addMoves()
void MoveGenerator::addMoves(std::vector<uint32_t>& moves, uint64_t pawns, uint64_t targetSquares, int shift) {
    while (targetSquares) {
        int targetSquare = popLSBIndex(targetSquares);
        int sourceSquare = targetSquare - shift;
        moves.push_back(MOVE(sourceSquare, targetSquare));
    }
}
