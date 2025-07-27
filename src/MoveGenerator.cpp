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
#include <random>
#include <array>

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

namespace Magic {
    using U64 = uint64_t;

    std::array<U64, 64> rookMasks{};
    std::array<U64, 64> bishopMasks{};
    std::array<U64, 64> rookMagics{};
    std::array<U64, 64> bishopMagics{};
    std::array<int, 64> rookShifts{};
    std::array<int, 64> bishopShifts{};
    std::array<std::vector<U64>, 64> rookTable{};
    std::array<std::vector<U64>, 64> bishopTable{};
    bool initialized = false;

    inline int popcount(U64 b) {
#if defined(_MSC_VER)
        return static_cast<int>(__popcnt64(b));
#else
        return __builtin_popcountll(b);
#endif
    }

    U64 random_u64() {
        static std::mt19937_64 gen(42);
        std::uniform_int_distribution<U64> dist(0, ~0ULL);
        return dist(gen);
    }

    U64 maskRook(int sq) {
        U64 mask = 0ULL;
        int r = sq / 8, f = sq % 8;
        for (int r1 = r + 1; r1 <= 6; ++r1) mask |= 1ULL << (r1 * 8 + f);
        for (int r1 = r - 1; r1 >= 1; --r1) mask |= 1ULL << (r1 * 8 + f);
        for (int f1 = f + 1; f1 <= 6; ++f1) mask |= 1ULL << (r * 8 + f1);
        for (int f1 = f - 1; f1 >= 1; --f1) mask |= 1ULL << (r * 8 + f1);
        return mask;
    }

    U64 maskBishop(int sq) {
        U64 mask = 0ULL;
        int r = sq / 8, f = sq % 8;
        for (int r1 = r + 1, f1 = f + 1; r1 <= 6 && f1 <= 6; ++r1, ++f1)
            mask |= 1ULL << (r1 * 8 + f1);
        for (int r1 = r + 1, f1 = f - 1; r1 <= 6 && f1 >= 1; ++r1, --f1)
            mask |= 1ULL << (r1 * 8 + f1);
        for (int r1 = r - 1, f1 = f + 1; r1 >= 1 && f1 <= 6; --r1, ++f1)
            mask |= 1ULL << (r1 * 8 + f1);
        for (int r1 = r - 1, f1 = f - 1; r1 >= 1 && f1 >= 1; --r1, --f1)
            mask |= 1ULL << (r1 * 8 + f1);
        return mask;
    }

    U64 rookAttacksOnTheFly(int sq, U64 occ) {
        U64 attacks = 0ULL;
        int r = sq / 8, f = sq % 8;
        for (int r1 = r + 1; r1 <= 7; ++r1) {
            int s = r1 * 8 + f; attacks |= 1ULL << s; if (occ & (1ULL << s)) break;
        }
        for (int r1 = r - 1; r1 >= 0; --r1) {
            int s = r1 * 8 + f; attacks |= 1ULL << s; if (occ & (1ULL << s)) break;
        }
        for (int f1 = f + 1; f1 <= 7; ++f1) {
            int s = r * 8 + f1; attacks |= 1ULL << s; if (occ & (1ULL << s)) break;
        }
        for (int f1 = f - 1; f1 >= 0; --f1) {
            int s = r * 8 + f1; attacks |= 1ULL << s; if (occ & (1ULL << s)) break;
        }
        return attacks;
    }

    U64 bishopAttacksOnTheFly(int sq, U64 occ) {
        U64 attacks = 0ULL;
        int r = sq / 8, f = sq % 8;
        for (int r1 = r + 1, f1 = f + 1; r1 <= 7 && f1 <= 7; ++r1, ++f1) {
            int s = r1 * 8 + f1; attacks |= 1ULL << s; if (occ & (1ULL << s)) break;
        }
        for (int r1 = r + 1, f1 = f - 1; r1 <= 7 && f1 >= 0; ++r1, --f1) {
            int s = r1 * 8 + f1; attacks |= 1ULL << s; if (occ & (1ULL << s)) break;
        }
        for (int r1 = r - 1, f1 = f + 1; r1 >= 0 && f1 <= 7; --r1, ++f1) {
            int s = r1 * 8 + f1; attacks |= 1ULL << s; if (occ & (1ULL << s)) break;
        }
        for (int r1 = r - 1, f1 = f - 1; r1 >= 0 && f1 >= 0; --r1, --f1) {
            int s = r1 * 8 + f1; attacks |= 1ULL << s; if (occ & (1ULL << s)) break;
        }
        return attacks;
    }

    U64 setOccupancy(int index, int bits, const std::vector<int>& squares) {
        U64 occ = 0ULL;
        for (int i = 0; i < bits; ++i)
            if (index & (1 << i))
                occ |= 1ULL << squares[i];
        return occ;
    }

    U64 findMagic(int sq, int bits, bool bishop) {
        std::vector<int> squares;
        U64 mask = bishop ? maskBishop(sq) : maskRook(sq);
        for (U64 m = mask; m; m &= m - 1)
            squares.push_back(popLSBIndex(m));

        int size = 1 << bits;
        std::vector<U64> occupancies(size), attacks(size);
        for (int i = 0; i < size; ++i) {
            occupancies[i] = setOccupancy(i, bits, squares);
            attacks[i] = bishop ? bishopAttacksOnTheFly(sq, occupancies[i])
                               : rookAttacksOnTheFly(sq, occupancies[i]);
        }

        while (true) {
            U64 magic = random_u64() & random_u64() & random_u64();

            if (popcount((mask * magic) & 0xFF00000000000000ULL) < 6) continue;

            std::vector<U64> used(size, 0ULL);
            bool fail = false;

            for (int i = 0; i < size && !fail; ++i) {
                int index = (int)((occupancies[i] * magic) >> (64 - bits));
                if (!used[index])
                    used[index] = attacks[i];
                else if (used[index] != attacks[i])
                    fail = true;
            }
            if (!fail)
                return magic;
        }
    }

    void init() {
        if (initialized) return;
        for (int sq = 0; sq < 64; ++sq) {
            rookMasks[sq] = maskRook(sq);
            bishopMasks[sq] = maskBishop(sq);

            int rBits = popcount(rookMasks[sq]);
            int bBits = popcount(bishopMasks[sq]);
            rookShifts[sq] = 64 - rBits;
            bishopShifts[sq] = 64 - bBits;

            rookTable[sq].resize(1ULL << rBits);
            bishopTable[sq].resize(1ULL << bBits);

            rookMagics[sq] = findMagic(sq, rBits, false);
            bishopMagics[sq] = findMagic(sq, bBits, true);
        }

        // Precompute attack tables
        for (int sq = 0; sq < 64; ++sq) {
            int rBits = popcount(rookMasks[sq]);
            int bBits = popcount(bishopMasks[sq]);
            std::vector<int> rSquares, bSquares;
            for (U64 m = rookMasks[sq]; m; m &= m - 1) rSquares.push_back(popLSBIndex(m));
            for (U64 m = bishopMasks[sq]; m; m &= m - 1) bSquares.push_back(popLSBIndex(m));

            for (int i = 0; i < (1 << rBits); ++i) {
                U64 occ = setOccupancy(i, rBits, rSquares);
                int index = (int)((occ * rookMagics[sq]) >> (64 - rBits));
                rookTable[sq][index] = rookAttacksOnTheFly(sq, occ);
            }
            for (int i = 0; i < (1 << bBits); ++i) {
                U64 occ = setOccupancy(i, bBits, bSquares);
                int index = (int)((occ * bishopMagics[sq]) >> (64 - bBits));
                bishopTable[sq][index] = bishopAttacksOnTheFly(sq, occ);
            }
        }

        initialized = true;
    }

    U64 getRookAttacks(int sq, U64 occ) {
        U64 masked = occ & rookMasks[sq];
        int index = (int)((masked * rookMagics[sq]) >> rookShifts[sq]);
        return rookTable[sq][index];
    }

    U64 getBishopAttacks(int sq, U64 occ) {
        U64 masked = occ & bishopMasks[sq];
        int index = (int)((masked * bishopMagics[sq]) >> bishopShifts[sq]);
        return bishopTable[sq][index];
    }
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

std::vector<std::string> MoveGenerator::generateKnightMoves(const Board& board, bool isWhite) {
    std::vector<std::string> moves;
    uint64_t knights = isWhite ? board.getWhiteKnights() : board.getBlackKnights();
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();

    const int offsets[8][2] = {
        {1, 2}, {2, 1}, {-1, 2}, {-2, 1},
        {1, -2}, {2, -1}, {-1, -2}, {-2, -1}
    };

    while (knights) {
        int from = popLSBIndex(knights);
        int fx = from % 8;
        int fy = from / 8;
        for (auto &o : offsets) {
            int tx = fx + o[0];
            int ty = fy + o[1];
            if (tx < 0 || tx > 7 || ty < 0 || ty > 7) continue;
            int to = ty * 8 + tx;
            uint64_t targetMask = 1ULL << to;
            if (targetMask & ownPieces) continue;
            moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }
    }

    return moves;
}

std::vector<std::string> MoveGenerator::generateRookMoves(const Board& board, bool isWhite) {
    Magic::init();
    std::vector<std::string> moves;
    uint64_t rooks = isWhite ? board.getWhiteRooks() : board.getBlackRooks();
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();
    uint64_t occupancy = board.getWhitePieces() | board.getBlackPieces();
    while (rooks) {
        int from = popLSBIndex(rooks);
        uint64_t attacks = Magic::getRookAttacks(from, occupancy) & ~ownPieces;
        for (uint64_t m = attacks; m; m &= m - 1) {
            int to = __builtin_ctzll(m);
            moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }
    }
    return moves;
}

std::vector<std::string> MoveGenerator::generateBishopMoves(const Board& board, bool isWhite) {
    Magic::init();
    std::vector<std::string> moves;
    uint64_t bishops = isWhite ? board.getWhiteBishops() : board.getBlackBishops();
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();
    uint64_t occupancy = board.getWhitePieces() | board.getBlackPieces();
    while (bishops) {
        int from = popLSBIndex(bishops);
        uint64_t attacks = Magic::getBishopAttacks(from, occupancy) & ~ownPieces;
        for (uint64_t m = attacks; m; m &= m - 1) {
            int to = __builtin_ctzll(m);
            moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }
    }
    return moves;
}

std::vector<std::string> MoveGenerator::generateQueenMoves(const Board& board, bool isWhite) {
    Magic::init();
    std::vector<std::string> moves;
    uint64_t queens = isWhite ? board.getWhiteQueens() : board.getBlackQueens();
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();
    uint64_t occupancy = board.getWhitePieces() | board.getBlackPieces();
    while (queens) {
        int from = popLSBIndex(queens);
        uint64_t attacks = (Magic::getRookAttacks(from, occupancy) | Magic::getBishopAttacks(from, occupancy)) & ~ownPieces;
        for (uint64_t m = attacks; m; m &= m - 1) {
            int to = __builtin_ctzll(m);
            moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }
    }
    return moves;
}


