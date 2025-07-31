#include "Engine.h"
#include "BitUtils.h"
#include "MoveGenerator.h"
#include <array>

Engine::GamePhase Engine::getGamePhase(const Board& b) const {
    uint64_t all = b.getWhitePieces() | b.getBlackPieces();
    int pieces = popcount64(all);
    if (pieces > 24) return GamePhase::Opening;
    if (pieces > 12) return GamePhase::Middlegame;
    return GamePhase::Endgame;
}

static int mirror(int sq) {
    return ((7 - (sq / 8)) * 8) + (sq % 8);
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
}

int Engine::evaluate(const Board& b) const {
    const int pawn = 100, knight = 320, bishop = 330, rook = 500, queen = 900, king = 20000;
    GamePhase phase = getGamePhase(b);
    int score = 0;
    uint64_t pieces;

    pieces = b.getWhitePawns();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score += pawn + pawnTable[sq];
    }
    pieces = b.getBlackPawns();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score -= pawn + pawnTable[mirror(sq)];
    }

    pieces = b.getWhiteKnights();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score += knight + knightTable[sq];
    }
    pieces = b.getBlackKnights();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score -= knight + knightTable[mirror(sq)];
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
    }
    pieces = b.getBlackRooks();
    while (pieces) {
        int sq = popLSBIndex(pieces);
        score -= rook + rookTable[mirror(sq)];
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

    return score;
}
