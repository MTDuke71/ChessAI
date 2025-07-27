#include "MoveGenerator.h"
#include "Board.h"
#include <vector>
#if defined(_MSC_VER)
#  include <intrin.h>
#else
#  include <cstdint>
#endif
#include <iostream>  // For printing moves
#include <string>

// Cross-platform bit scan (find least significant bit index)
inline int popLSBIndex(uint64_t& bitboard) {
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanForward64(&index, bitboard);
    bitboard &= bitboard - 1;
    return static_cast<int>(index);
#else
    int index = __builtin_ctzll(bitboard);
    bitboard &= bitboard - 1;
    return index;
#endif
}

// Converts square index (0-63) to chess notation (e.g., 8 -> "e2")
std::string squareToNotation(int square) {
    char file = 'a' + (square % 8);
    char rank = '1' + (square / 8);
    return std::string{ file } + rank;
}

// Converts board index (0-63) to algebraic notation (e.g., e2, d4)
 std::string indexToAlgebraic(int index) {
    char file = 'a' + (index % 8);
    int rank = (index / 8) + 1;
    return std::string(1, file) + std::to_string(rank);
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
     std::cout << "En Passant Square Bitboard: " << std::hex << (1ULL << board.getEnPassantSquare()) << "\n";

     uint64_t whitePawns = board.getWhitePawns();
     uint64_t blackPawns = board.getBlackPawns();
     uint64_t enPassantSquareBitboard = 1ULL << board.getEnPassantSquare();

     printBitboard(whitePawns, "White Pawns");
     printBitboard(blackPawns, "Black Pawns");

    uint64_t fromMask = isWhite
        ? ((enPassantSquareBitboard >> 9) & whitePawns & 0xFEFEFEFEFEFEFEFEULL) |
          ((enPassantSquareBitboard >> 7) & whitePawns & 0x7F7F7F7F7F7F7F7FULL)
        : ((enPassantSquareBitboard << 7) & blackPawns & 0xFEFEFEFEFEFEFEFEULL) |
          ((enPassantSquareBitboard << 9) & blackPawns & 0x7F7F7F7F7F7F7F7FULL);

    std::cout << "Eligible Pawns Bitboard: " << std::hex << fromMask << "\n";
    printBitboard(fromMask, "Eligible Pawns");

    if (fromMask) {
        std::cout << "Valid En Passant Capture Found!\n";
    } else {
        std::cout << "No valid en passant capture found.\n";
    }

     std::cout << "==================================\n";
 }

 std::vector<std::string> MoveGenerator::generatePawnMoves(const Board& board, bool isWhite) {
     std::vector<std::string> moves;
     uint64_t pawns = isWhite ? board.getWhitePawns() : board.getBlackPawns();
     uint64_t opponentPieces = isWhite ? board.getBlackPieces() : board.getWhitePieces();
     uint64_t emptySquares = ~(board.getWhitePieces() | board.getBlackPieces());

     // Pawn Promotion Logic
     uint64_t promotionRank = isWhite ? 0xFF00000000000000 : 0x00000000000000FF;
     uint64_t promotionPushes = (pawns << 8) & promotionRank & emptySquares;
    for (int pos = 0; promotionPushes; promotionPushes &= promotionPushes - 1) {
#if defined(_MSC_VER)
        pos = static_cast<int>(_tzcnt_u64(promotionPushes));
#else
        pos = __builtin_ctzll(promotionPushes);
#endif
        moves.push_back(indexToAlgebraic(pos - 8) + "-" + indexToAlgebraic(pos) + " (Promotes to Queen)");
    }

     // Pawn Capture Logic with Promotion Support
     uint64_t captureMoves = (pawns << 7) & opponentPieces & promotionRank & 0x7F7F7F7F7F7F7F7F;
     captureMoves |= (pawns << 9) & opponentPieces & promotionRank & 0xFEFEFEFEFEFEFEFE;
    for (int pos = 0; captureMoves; captureMoves &= captureMoves - 1) {
#if defined(_MSC_VER)
        pos = static_cast<int>(_tzcnt_u64(captureMoves));
#else
        pos = __builtin_ctzll(captureMoves);
#endif
        moves.push_back(indexToAlgebraic(pos - 8) + "-" + indexToAlgebraic(pos) + " (Captures and Promotes)");
    }

    if (board.getEnPassantSquare() != -1) {
        debugEnPassant(board, isWhite);

        uint64_t epSquare = 1ULL << board.getEnPassantSquare();
        uint64_t fromMask;
        if (isWhite) {
            fromMask = ((epSquare >> 9) & pawns & 0xFEFEFEFEFEFEFEFEULL) |
                       ((epSquare >> 7) & pawns & 0x7F7F7F7F7F7F7F7FULL);
        } else {
            fromMask = ((epSquare << 7) & pawns & 0xFEFEFEFEFEFEFEFEULL) |
                       ((epSquare << 9) & pawns & 0x7F7F7F7F7F7F7F7FULL);
        }

        for (uint64_t mask = fromMask; mask; mask &= mask - 1) {
            int from =
#if defined(_MSC_VER)
                static_cast<int>(_tzcnt_u64(mask));
#else
                __builtin_ctzll(mask);
#endif
            int to = board.getEnPassantSquare();
            moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + " (En Passant)");
        }
    }

     return moves;
 }



void MoveGenerator::addMoves(std::vector<std::string>& moves, uint64_t pawns, uint64_t moveBoard, int shift) {
    for (int from = 0; moveBoard; moveBoard &= moveBoard - 1) {
        int to =
#if defined(_MSC_VER)
            static_cast<int>(_tzcnt_u64(moveBoard)); // Target square
#else
            __builtin_ctzll(moveBoard);
#endif
        from = to - shift; // Calculate starting square
        moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
    }
}


