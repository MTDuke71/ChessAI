#include "Zobrist.h"
#include <random>

namespace Zobrist {
    std::array<std::array<uint64_t,64>,12> pieceHashes;
    uint64_t sideHash;
    std::array<uint64_t,4> castleHash;
    std::array<uint64_t,8> enPassantHash;

    void init() {
        std::mt19937_64 rng(0xABCDEF); // fixed seed for reproducibility
        for (auto& arr : pieceHashes)
            for (auto& v : arr) v = rng();
        sideHash = rng();
        for (auto& v : castleHash) v = rng();
        for (auto& v : enPassantHash) v = rng();
    }

    uint64_t hashBoard(const Board& b) {
        uint64_t h = 0;
        auto addPieces = [&](uint64_t bb, int pieceIndex) {
            while (bb) {
                int sq;
#if defined(_MSC_VER)
                unsigned long idx; _BitScanForward64(&idx, bb); sq = static_cast<int>(idx);
                bb &= bb - 1;
#else
                sq = __builtin_ctzll(bb); bb &= bb - 1ULL;
#endif
                h ^= pieceHashes[pieceIndex][sq];
            }
        };
        addPieces(b.getWhitePawns(), 0);
        addPieces(b.getWhiteKnights(), 1);
        addPieces(b.getWhiteBishops(), 2);
        addPieces(b.getWhiteRooks(), 3);
        addPieces(b.getWhiteQueens(), 4);
        addPieces(b.getWhiteKing(), 5);
        addPieces(b.getBlackPawns(), 6);
        addPieces(b.getBlackKnights(), 7);
        addPieces(b.getBlackBishops(), 8);
        addPieces(b.getBlackRooks(), 9);
        addPieces(b.getBlackQueens(),10);
        addPieces(b.getBlackKing(), 11);
        if (b.isWhiteToMove()) h ^= sideHash;
        if (b.canCastleWK()) h ^= castleHash[0];
        if (b.canCastleWQ()) h ^= castleHash[1];
        if (b.canCastleBK()) h ^= castleHash[2];
        if (b.canCastleBQ()) h ^= castleHash[3];
        int ep = b.getEnPassantSquare();
        if (ep != -1) h ^= enPassantHash[ep % 8];
        return h;
    }
}

