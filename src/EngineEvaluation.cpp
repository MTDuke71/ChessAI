#include "Engine.h"
#include "BitUtils.h"
#include "MoveGenerator.h"
#include <array>

// -----------------------------------------------------------------------------
// Determines the phase of the game (opening, middlegame, endgame) based on the
// total number of pieces remaining on the board.
// -----------------------------------------------------------------------------
Engine::GamePhase Engine::getGamePhase(const Board& b) const {
    uint64_t all = b.getWhitePieces() | b.getBlackPieces();
    int pieces = popcount64(all);
    if (pieces > 24) return GamePhase::Opening;
    if (pieces > 12) return GamePhase::Middlegame;
    return GamePhase::Endgame;
}

// -----------------------------------------------------------------------------
// Returns the mirrored square index relative to the horizontal center of the
// board. Useful for evaluating black piece positions using white tables.
// -----------------------------------------------------------------------------
static int mirror(int sq) {
    return ((7 - (sq / 8)) * 8) + (sq % 8);
}

// -----------------------------------------------------------------------------
// Generates a bitboard mask of all squares attacked by a king placed on the
// given square.
// -----------------------------------------------------------------------------
static uint64_t kingAttackMask(int sq) {
    int r = sq / 8, f = sq % 8;
    uint64_t mask = 0;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int df = -1; df <= 1; ++df) {
            if (dr == 0 && df == 0) continue;
            int tr = r + dr, tf = f + df;
            if (tr >= 0 && tr < 8 && tf >= 0 && tf < 8)
                mask |= 1ULL << (tr * 8 + tf);
        }
    }
    return mask;
}

namespace {
const std::array<int, 64> pawnTable = {
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10,-20,-20, 10, 10,  5,
     5, -5,-10,  0,  0,-10, -5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5,  5, 10, 25, 25, 10,  5,  5,
    10, 10, 20, 30, 30, 20, 10, 10,
    50, 50, 50, 50, 50, 50, 50, 50,
     0,  0,  0,  0,  0,  0,  0,  0
};

const std::array<int, 64> knightTable = {
   -50,-40,-30,-30,-30,-30,-40,-50,
   -40,-20,  0,  0,  0,  0,-20,-40,
   -30,  0, 10, 15, 15, 10,  0,-30,
   -30,  5, 15, 20, 20, 15,  5,-30,
   -30,  0, 15, 20, 20, 15,  0,-30,
   -30,  5, 10, 15, 15, 10,  5,-30,
   -40,-20,  0,  5,  5,  0,-20,-40,
   -50,-40,-30,-30,-30,-30,-40,-50
};

const std::array<int, 64> bishopTable = {
   -20,-10,-10,-10,-10,-10,-10,-20,
   -10,  0,  0,  0,  0,  0,  0,-10,
   -10,  0,  5, 10, 10,  5,  0,-10,
   -10,  5,  5, 10, 10,  5,  5,-10,
   -10,  0, 10, 10, 10, 10,  0,-10,
   -10, 10, 10, 10, 10, 10, 10,-10,
   -10,  5,  0,  0,  0,  0,  5,-10,
   -20,-10,-10,-10,-10,-10,-10,-20
};

const std::array<int, 64> rookTable = {
     0,  0,  0,  5,  5,  0,  0,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     5, 10, 10, 10, 10, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

const std::array<int, 64> queenTable = {
   -20,-10,-10, -5, -5,-10,-10,-20,
   -10,  0,  0,  0,  0,  0,  0,-10,
   -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
     0,  0,  5,  5,  5,  5,  0, -5,
   -10,  5,  5,  5,  5,  5,  0,-10,
   -10,  0,  5,  0,  0,  0,  0,-10,
   -20,-10,-10, -5, -5,-10,-10,-20
};

const std::array<int, 64> kingTable = {
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -20,-30,-30,-40,-40,-30,-30,-20,
   -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};

const std::array<int, 64> kingTableEndgame = {
   -50,-40,-30,-20,-20,-30,-40,-50,
   -30,-20,-10,  0,  0,-10,-20,-30,
   -30,-10, 20, 30, 30, 20,-10,-30,
   -30,-10, 30, 40, 40, 30,-10,-30,
   -30,-10, 30, 40, 40, 30,-10,-30,
   -30,-10, 20, 30, 30, 20,-10,-30,
   -30,-30,  0,  0,  0,  0,-30,-30,
   -50,-30,-30,-30,-30,-30,-30,-50
};

// Bonus for passed pawns by rank (from White's perspective)
const std::array<int,8> passedPawnBonus = {0, 5, 10, 20, 35, 60, 100, 0};
}

// -----------------------------------------------------------------------------
// Evaluates the given board position and returns a score. Positive values favor
// White, negative values favor Black.
// -----------------------------------------------------------------------------
int Engine::evaluate(const Board& b) const {
    const int pawn = 100, knight = 320, bishop = 330, rook = 500, queen = 900, king = 20000;
    GamePhase phase = getGamePhase(b);
    int score = 0;
    uint64_t pieces;

    auto isWhitePassed = [&b](int sq) {
        int rank = sq / 8;
        int file = sq % 8;
        int startFile = (file > 0) ? file - 1 : file;
        int endFile = (file < 7) ? file + 1 : file;
        uint64_t blackPawns = b.getBlackPawns();
        for (int r = rank + 1; r < 8; ++r) {
            for (int f = startFile; f <= endFile; ++f) {
                int idx = r * 8 + f;
                if (blackPawns & (1ULL << idx)) return false;
            }
        }
        return true;
    };
    auto isBlackPassed = [&b](int sq) {
        int rank = sq / 8;
        int file = sq % 8;
        int startFile = (file > 0) ? file - 1 : file;
        int endFile = (file < 7) ? file + 1 : file;
        uint64_t whitePawns = b.getWhitePawns();
        for (int r = rank - 1; r >= 0; --r) {
            for (int f = startFile; f <= endFile; ++f) {
                int idx = r * 8 + f;
                if (whitePawns & (1ULL << idx)) return false;
            }
        }
        return true;
    };

    auto isWhiteKnightOutpost = [&b](int sq) {
        int rank = sq / 8;
        if (rank < 3) return false; // needs to be on 4th rank or beyond
        int file = sq % 8;
        uint64_t blackPawns = b.getBlackPawns();
        uint64_t whitePawns = b.getWhitePawns();
        uint64_t attackers = 0, supporters = 0;
        if (file > 0) {
            attackers |= 1ULL << (sq + 7);
            supporters |= 1ULL << (sq - 9);
        }
        if (file < 7) {
            attackers |= 1ULL << (sq + 9);
            supporters |= 1ULL << (sq - 7);
        }
        if (blackPawns & attackers) return false;
        if ((whitePawns & supporters) == 0) return false;
        return true;
    };

    auto isBlackKnightOutpost = [&b](int sq) {
        int rank = sq / 8;
        if (rank > 4) return false; // needs to be on 5th rank or beyond from black's view
        int file = sq % 8;
        uint64_t whitePawns = b.getWhitePawns();
        uint64_t blackPawns = b.getBlackPawns();
        uint64_t attackers = 0, supporters = 0;
        if (file > 0) {
            attackers |= 1ULL << (sq - 9);
            supporters |= 1ULL << (sq + 7);
        }
        if (file < 7) {
            attackers |= 1ULL << (sq - 7);
            supporters |= 1ULL << (sq + 9);
        }
        if (whitePawns & attackers) return false;
        if ((blackPawns & supporters) == 0) return false;
        return true;
    };

    pieces = b.getWhitePawns();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score += pawn + pawnTable[sq];
        if (isWhitePassed(sq))
            score += passedPawnBonus[sq / 8];
    }
    pieces = b.getBlackPawns();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score -= pawn + pawnTable[mirror(sq)];
        if (isBlackPassed(sq))
            score -= passedPawnBonus[7 - (sq / 8)];
    }

    pieces = b.getWhiteKnights();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score += knight + knightTable[sq];
        if (isWhiteKnightOutpost(sq))
            score += 25;
    }
    pieces = b.getBlackKnights();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score -= knight + knightTable[mirror(sq)];
        if (isBlackKnightOutpost(sq))
            score -= 25;
    }

    pieces = b.getWhiteBishops();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score += bishop + bishopTable[sq];
    }
    if (popcount64(b.getWhiteBishops()) >= 2)
        score += 50; // bishop pair bonus
    pieces = b.getBlackBishops();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score -= bishop + bishopTable[mirror(sq)];
    }
    if (popcount64(b.getBlackBishops()) >= 2)
        score -= 50; // bishop pair bonus

    pieces = b.getWhiteRooks();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score += rook + rookTable[sq];
        uint64_t fileMask = 0x0101010101010101ULL << (sq % 8);
        if (((b.getWhitePawns() | b.getBlackPawns()) & fileMask) == 0)
            score += 15;
    }
    pieces = b.getBlackRooks();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score -= rook + rookTable[mirror(sq)];
        uint64_t fileMask = 0x0101010101010101ULL << (sq % 8);
        if (((b.getWhitePawns() | b.getBlackPawns()) & fileMask) == 0)
            score -= 15;
    }

    pieces = b.getWhiteQueens();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score += queen + queenTable[sq];
    }
    pieces = b.getBlackQueens();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score -= queen + queenTable[mirror(sq)];
    }

    const std::array<int,64>* kingTablePtr =
            (phase == GamePhase::Endgame) ? &kingTableEndgame : &kingTable;
    pieces = b.getWhiteKing();
    if (pieces) {
        int sq = lsbIndex(pieces);
        score += king + (*kingTablePtr)[sq];
    }
    pieces = b.getBlackKing();
    if (pieces) {
        int sq = lsbIndex(pieces);
        score -= king + (*kingTablePtr)[mirror(sq)];
    }

    int mobilityWeight = 5;
    int developBonus = 15;
    if (phase == GamePhase::Opening) {
        developBonus = 20;
    } else if (phase == GamePhase::Endgame) {
        mobilityWeight = 2;
        developBonus = 0;
    }
    int whiteMobility = static_cast<int>(generator.generateAllMoves(b, true).size());
    int blackMobility = static_cast<int>(generator.generateAllMoves(b, false).size());
    score += mobilityWeight * (whiteMobility - blackMobility);

    auto countDeveloped = [](uint64_t pieces, const std::initializer_list<int>& starts) {
        int developed = 0;
        for (uint64_t bb = pieces; bb; bb &= bb - 1) {
            int sq = lsbIndex(bb);
            bool onStart = false;
            for (int s : starts) if (sq == s) { onStart = true; break; }
            if (!onStart) ++developed;
        }
        return developed;
    };

    int whiteDevelop = 0, blackDevelop = 0;
    whiteDevelop += countDeveloped(b.getWhiteKnights(), {1,6});
    whiteDevelop += countDeveloped(b.getWhiteBishops(), {2,5});
    blackDevelop += countDeveloped(b.getBlackKnights(), {57,62});
    blackDevelop += countDeveloped(b.getBlackBishops(), {58,61});
    score += developBonus * (whiteDevelop - blackDevelop);

    auto kingSafety = [&](bool white) {
        uint64_t kingBb = white ? b.getWhiteKing() : b.getBlackKing();
        if (!kingBb) return 0;
        int sq = lsbIndex(kingBb);
        uint64_t pawns = white ? b.getWhitePawns() : b.getBlackPawns();
        uint64_t shield = 0;
        int file = sq % 8;
        if (white) {
            if (sq / 8 < 7) {
                shield |= 1ULL << (sq + 8);
                if (file > 0) shield |= 1ULL << (sq + 7);
                if (file < 7) shield |= 1ULL << (sq + 9);
            }
        } else {
            if (sq / 8 > 0) {
                shield |= 1ULL << (sq - 8);
                if (file > 0) shield |= 1ULL << (sq - 9);
                if (file < 7) shield |= 1ULL << (sq - 7);
            }
        }
        int shieldCount = popcount64(pawns & shield);
        int score = 10 * shieldCount;
        uint64_t area = kingAttackMask(sq);
        int attacked = 0;
        for (uint64_t m = area; m; m &= m - 1) {
            int s = popLSBIndex(m);
            if (generator.isSquareAttacked(b, s, !white))
                ++attacked;
        }
        score -= 15 * attacked;
        return score;
    };

    score += kingSafety(true);
    score -= kingSafety(false);

    if (phase != GamePhase::Endgame) {
        bool whiteCastled = (b.getWhiteKing() == (1ULL<<6)) ||
                            (b.getWhiteKing() == (1ULL<<2));
        bool blackCastled = (b.getBlackKing() == (1ULL<<62)) ||
                            (b.getBlackKing() == (1ULL<<58));
        bool whiteHome = b.getWhiteKing() == (1ULL<<4);
        bool blackHome = b.getBlackKing() == (1ULL<<60);
        int castleBonus = 40;
        int stuckPenalty = 20;
        if (whiteCastled)
            score += castleBonus;
        else if (whiteHome && !b.canCastleWK() && !b.canCastleWQ())
            score -= stuckPenalty;
        if (blackCastled)
            score -= castleBonus;
        else if (blackHome && !b.canCastleBK() && !b.canCastleBQ())
            score += stuckPenalty;
    }

    return score;
}
