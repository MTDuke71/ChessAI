#include "MoveGenerator.h"
#include "Board.h"
#include <vector>
#include <intrin.h>  // For _tzcnt_u64 in Visual Studio
#include <iostream>  // For printing moves
#include <string>

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

     uint64_t eligiblePawns = isWhite
         ? ((whitePawns >> 1) & 0x7F7F7F7F7F7F7F7F & enPassantSquareBitboard) |
         ((whitePawns << 1) & 0xFEFEFEFEFEFEFEFE & enPassantSquareBitboard)
         : ((blackPawns >> 1) & 0x7F7F7F7F7F7F7F7F & enPassantSquareBitboard) |
         ((blackPawns << 1) & 0xFEFEFEFEFEFEFEFE & enPassantSquareBitboard);

     std::cout << "Eligible Pawns Bitboard: " << std::hex << eligiblePawns << "\n";
     printBitboard(eligiblePawns, "Eligible Pawns");

     uint64_t validEnPassantMask = isWhite
         ? (enPassantSquareBitboard >> 8) & eligiblePawns
         : (enPassantSquareBitboard << 8) & eligiblePawns;

     std::cout << "Valid En Passant Mask: " << std::hex << validEnPassantMask << "\n";
     printBitboard(validEnPassantMask, "Valid En Passant Mask");

     if (validEnPassantMask) {
         std::cout << "Valid En Passant Capture Found!\n";
     }
     else {
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
         pos = static_cast<int>(_tzcnt_u64(promotionPushes));
         moves.push_back(indexToAlgebraic(pos - 8) + "-" + indexToAlgebraic(pos) + " (Promotes to Queen)");
     }

     // Pawn Capture Logic with Promotion Support
     uint64_t captureMoves = (pawns << 7) & opponentPieces & promotionRank & 0x7F7F7F7F7F7F7F7F;
     captureMoves |= (pawns << 9) & opponentPieces & promotionRank & 0xFEFEFEFEFEFEFEFE;
     for (int pos = 0; captureMoves; captureMoves &= captureMoves - 1) {
         pos = static_cast<int>(_tzcnt_u64(captureMoves));
         moves.push_back(indexToAlgebraic(pos - 8) + "-" + indexToAlgebraic(pos) + " (Captures and Promotes)");
     }

     if (board.getEnPassantSquare() != -1) {
         debugEnPassant(board, isWhite);

         uint64_t enPassantSquare = 1ULL << board.getEnPassantSquare();
         uint64_t eligiblePawns = isWhite
             ? ((pawns >> 1) & 0x7F7F7F7F7F7F7F7F & enPassantSquare) |
             ((pawns << 1) & 0xFEFEFEFEFEFEFEFE & enPassantSquare)
             : ((pawns >> 1) & 0x7F7F7F7F7F7F7F7F & enPassantSquare) |
             ((pawns << 1) & 0xFEFEFEFEFEFEFEFE & enPassantSquare);

         uint64_t validEnPassantMask = isWhite
             ? (enPassantSquare >> 8) & eligiblePawns
             : (enPassantSquare << 8) & eligiblePawns;

         if (validEnPassantMask) {
             int from = static_cast<int>(_tzcnt_u64(validEnPassantMask));
             int to = board.getEnPassantSquare();
             moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + " (En Passant)");
         }
     }

     return moves;
 }



void MoveGenerator::addMoves(std::vector<std::string>& moves, uint64_t pawns, uint64_t moveBoard, int shift) {
    for (int from = 0; moveBoard; moveBoard &= moveBoard - 1) {
        int to = static_cast<int>(_tzcnt_u64(moveBoard)); // Target square
        from = to - shift; // Calculate starting square
        moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
    }
}


