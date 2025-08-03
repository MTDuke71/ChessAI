#include "Magic.h"
#include "BitUtils.h"
#include <vector>
#include <array>
#include <random>

//------------------------------------------------------------------------------
// Magic bitboard generation for sliding piece attacks. This module precomputes
// masks, magic numbers and lookup tables for rooks and bishops to allow fast
// move generation during search.
//------------------------------------------------------------------------------
namespace Magic {
    std::array<U64, 64> rookMasks{};
    std::array<U64, 64> bishopMasks{};
    std::array<U64, 64> rookMagics{};
    std::array<U64, 64> bishopMagics{};
    std::array<int, 64> rookShifts{};
    std::array<int, 64> bishopShifts{};
    std::array<std::vector<U64>, 64> rookTable{};
    std::array<std::vector<U64>, 64> bishopTable{};
    bool initialized = false;

    //------------------------------------------------------------------------------
    // Return the number of set bits in a 64-bit integer.
    //------------------------------------------------------------------------------
    inline int popcount(U64 b) {
        return popcount64(b);
    }

    //------------------------------------------------------------------------------
    // Generate a pseudo-random 64-bit value used during magic number search.
    //------------------------------------------------------------------------------
    U64 random_u64() {
        static std::mt19937_64 gen(42);
        std::uniform_int_distribution<U64> dist(0, ~0ULL);
        return dist(gen);
    }

    //------------------------------------------------------------------------------
    // Compute the rook attack mask for the given square on an otherwise empty board.
    //------------------------------------------------------------------------------
    U64 maskRook(int sq) {
        U64 mask = 0ULL;
        int r = sq / 8, f = sq % 8;
        for (int r1 = r + 1; r1 <= 6; ++r1) mask |= 1ULL << (r1 * 8 + f);
        for (int r1 = r - 1; r1 >= 1; --r1) mask |= 1ULL << (r1 * 8 + f);
        for (int f1 = f + 1; f1 <= 6; ++f1) mask |= 1ULL << (r * 8 + f1);
        for (int f1 = f - 1; f1 >= 1; --f1) mask |= 1ULL << (r * 8 + f1);
        return mask;
    }

    //------------------------------------------------------------------------------
    // Compute the bishop attack mask for the given square on an otherwise empty board.
    //------------------------------------------------------------------------------
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

    //------------------------------------------------------------------------------
    // Generate rook attack bitboard on the fly given a square and occupancy.
    //------------------------------------------------------------------------------
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

    //------------------------------------------------------------------------------
    // Generate bishop attack bitboard on the fly given a square and occupancy.
    //------------------------------------------------------------------------------
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

    //------------------------------------------------------------------------------
    // Build an occupancy bitboard from an index and a list of attack squares.
    //------------------------------------------------------------------------------
    U64 setOccupancy(int index, int bits, const std::vector<int>& squares) {
        U64 occ = 0ULL;
        for (int i = 0; i < bits; ++i)
            if (index & (1 << i))
                occ |= 1ULL << squares[i];
        return occ;
    }

    //------------------------------------------------------------------------------
    // Search for a suitable magic number for the given square and piece type.
    //------------------------------------------------------------------------------
    U64 findMagic(int sq, int bits, bool bishop) {
        std::vector<int> squares;
        U64 mask = bishop ? maskBishop(sq) : maskRook(sq);
        for (U64 m = mask; m; m &= m - 1)
            squares.push_back(lsbIndex(m));

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

    //------------------------------------------------------------------------------
    // Initialize magic numbers, masks, shifts and attack tables.
    //------------------------------------------------------------------------------
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
            for (U64 m = rookMasks[sq]; m; m &= m - 1) rSquares.push_back(lsbIndex(m));
            for (U64 m = bishopMasks[sq]; m; m &= m - 1) bSquares.push_back(lsbIndex(m));

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

    //------------------------------------------------------------------------------
    // Retrieve rook attack bitboard from the precomputed table.
    //------------------------------------------------------------------------------
    U64 getRookAttacks(int sq, U64 occ) {
        U64 masked = occ & rookMasks[sq];
        int index = (int)((masked * rookMagics[sq]) >> rookShifts[sq]);
        return rookTable[sq][index];
    }

    //------------------------------------------------------------------------------
    // Retrieve bishop attack bitboard from the precomputed table.
    //------------------------------------------------------------------------------
    U64 getBishopAttacks(int sq, U64 occ) {
        U64 masked = occ & bishopMasks[sq];
        int index = (int)((masked * bishopMagics[sq]) >> bishopShifts[sq]);
        return bishopTable[sq][index];
    }
}
