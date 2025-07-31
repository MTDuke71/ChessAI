#include "MoveGenerator.h"
#include "Board.h"
#include "Magic.h"
#include <vector>
#include "BitUtils.h"
#if defined(_MSC_VER)
#  include <intrin.h>
#else
#  include <cstdint>
#endif
#include <iostream>  // For printing moves
#include <string>
#include <random>
#include <array>




MoveGenerator::MoveGenerator() {
    Magic::init();
}

std::vector<std::string> MoveGenerator::generatePawnMoves(const Board& board, bool isWhite) const {
    std::vector<std::string> moves;
    uint64_t pawns = isWhite ? board.getWhitePawns() : board.getBlackPawns();
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();
    uint64_t opponentPieces = isWhite ? board.getBlackPieces() : board.getWhitePieces();
    uint64_t emptySquares = ~(board.getWhitePieces() | board.getBlackPieces());

    const uint64_t whiteStartRank = 0x000000000000FF00ULL;
    const uint64_t blackStartRank = 0x00FF000000000000ULL;
    const uint64_t whitePromRank  = 0xFF00000000000000ULL;
    const uint64_t blackPromRank  = 0x00000000000000FFULL;

    if (isWhite) {
        // Single pushes
        uint64_t one = (pawns << 8) & emptySquares;
        for (uint64_t targets = one; targets; targets &= targets - 1) {
            int to = lsbIndex(targets);
            int from = to - 8;
            if ((1ULL << to) & whitePromRank)
                moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + " (Promotes to Queen)");
            else
                moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }

        // Double pushes from starting rank
        uint64_t two = (((pawns & whiteStartRank) << 8) & emptySquares) << 8;
        two &= emptySquares;
        for (uint64_t targets = two; targets; targets &= targets - 1) {
            int to = lsbIndex(targets);
            int from = to - 16;
            moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }

        // Captures
        uint64_t left = (pawns << 9) & opponentPieces & 0xFEFEFEFEFEFEFEFEULL;
        for (uint64_t targets = left; targets; targets &= targets - 1) {
            int to = lsbIndex(targets);
            int from = to - 9;
            if ((1ULL << to) & whitePromRank)
                moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + " (Captures and Promotes)");
            else
                moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }

        uint64_t right = (pawns << 7) & opponentPieces & 0x7F7F7F7F7F7F7F7FULL;
        for (uint64_t targets = right; targets; targets &= targets - 1) {
            int to = lsbIndex(targets);
            int from = to - 7;
            if ((1ULL << to) & whitePromRank)
                moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + " (Captures and Promotes)");
            else
                moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }
    } else {
        // Single pushes
        uint64_t one = (pawns >> 8) & emptySquares;
        for (uint64_t targets = one; targets; targets &= targets - 1) {
            int to = lsbIndex(targets);
            int from = to + 8;
            if ((1ULL << to) & blackPromRank)
                moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + " (Promotes to Queen)");
            else
                moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }

        // Double pushes from starting rank
        uint64_t two = (((pawns & blackStartRank) >> 8) & emptySquares) >> 8;
        two &= emptySquares;
        for (uint64_t targets = two; targets; targets &= targets - 1) {
            int to = lsbIndex(targets);
            int from = to + 16;
            moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }

        // Captures
        uint64_t left = (pawns >> 7) & opponentPieces & 0xFEFEFEFEFEFEFEFEULL;
        for (uint64_t targets = left; targets; targets &= targets - 1) {
            int to = lsbIndex(targets);
            int from = to + 7;
            if ((1ULL << to) & blackPromRank)
                moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + " (Captures and Promotes)");
            else
                moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }

        uint64_t right = (pawns >> 9) & opponentPieces & 0x7F7F7F7F7F7F7F7FULL;
        for (uint64_t targets = right; targets; targets &= targets - 1) {
            int to = lsbIndex(targets);
            int from = to + 9;
            if ((1ULL << to) & blackPromRank)
                moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + " (Captures and Promotes)");
            else
                moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }
    }

    // En passant
    if (board.getEnPassantSquare() != -1) {
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
            int from = lsbIndex(mask);
            int to = board.getEnPassantSquare();
            moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + " (En Passant)");
        }
    }

    return moves;
}



void MoveGenerator::addMoves(std::vector<std::string>& moves, uint64_t pawns, uint64_t moveBoard, int shift) const {
    for (int from = 0; moveBoard; moveBoard &= moveBoard - 1) {
        int to = lsbIndex(moveBoard);
        from = to - shift; // Calculate starting square
        moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
    }
}

std::vector<std::string> MoveGenerator::generateKnightMoves(const Board& board, bool isWhite) const {
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

std::vector<std::string> MoveGenerator::generateRookMoves(const Board& board, bool isWhite) const {
    std::vector<std::string> moves;
    uint64_t rooks = isWhite ? board.getWhiteRooks() : board.getBlackRooks();
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();
    uint64_t occupancy = board.getWhitePieces() | board.getBlackPieces();
    while (rooks) {
        int from = popLSBIndex(rooks);
        uint64_t attacks = Magic::getRookAttacks(from, occupancy) & ~ownPieces;
        for (uint64_t m = attacks; m; m &= m - 1) {
            int to = lsbIndex(m);
            moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }
    }
    return moves;
}

std::vector<std::string> MoveGenerator::generateBishopMoves(const Board& board, bool isWhite) const {
    std::vector<std::string> moves;
    uint64_t bishops = isWhite ? board.getWhiteBishops() : board.getBlackBishops();
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();
    uint64_t occupancy = board.getWhitePieces() | board.getBlackPieces();
    while (bishops) {
        int from = popLSBIndex(bishops);
        uint64_t attacks = Magic::getBishopAttacks(from, occupancy) & ~ownPieces;
        for (uint64_t m = attacks; m; m &= m - 1) {
            int to = lsbIndex(m);
            moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }
    }
    return moves;
}

std::vector<std::string> MoveGenerator::generateQueenMoves(const Board& board, bool isWhite) const {
    std::vector<std::string> moves;
    uint64_t queens = isWhite ? board.getWhiteQueens() : board.getBlackQueens();
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();
    uint64_t occupancy = board.getWhitePieces() | board.getBlackPieces();
    while (queens) {
        int from = popLSBIndex(queens);
        uint64_t attacks = (Magic::getRookAttacks(from, occupancy) | Magic::getBishopAttacks(from, occupancy)) & ~ownPieces;
        for (uint64_t m = attacks; m; m &= m - 1) {
            int to = lsbIndex(m);
            moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }
    }
    return moves;
}

std::vector<std::string> MoveGenerator::generateKingMoves(const Board& board, bool isWhite) const {
    std::vector<std::string> moves;
    uint64_t king = isWhite ? board.getWhiteKing() : board.getBlackKing();
    if (!king) return moves;
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();

    int from = lsbIndex(king);
    int fx = from % 8; int fy = from / 8;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            int tx = fx + dx; int ty = fy + dy;
            if (tx < 0 || tx > 7 || ty < 0 || ty > 7) continue;
            int to = ty * 8 + tx;
            uint64_t mask = 1ULL << to;
            if (mask & ownPieces) continue;
            moves.push_back(indexToAlgebraic(from) + "-" + indexToAlgebraic(to));
        }
    }

    uint64_t allPieces = board.getWhitePieces() | board.getBlackPieces();
    if (isWhite) {
        if (board.canCastleWK() && (from == 4) &&
            !(allPieces & ((1ULL<<5) | (1ULL<<6))) &&
            (board.getWhiteRooks() & (1ULL<<7)) &&
            !isKingInCheck(board, true) &&
            !isSquareAttacked(board, 5, false) &&
            !isSquareAttacked(board, 6, false)) {
            moves.push_back("e1-g1 (Castle Kingside)");
        }
        if (board.canCastleWQ() && (from == 4) &&
            !(allPieces & ((1ULL<<1)|(1ULL<<2)|(1ULL<<3))) &&
            (board.getWhiteRooks() & (1ULL<<0)) &&
            !isKingInCheck(board, true) &&
            !isSquareAttacked(board, 3, false) &&
            !isSquareAttacked(board, 2, false)) {
            moves.push_back("e1-c1 (Castle Queenside)");
        }
    } else {
        if (board.canCastleBK() && (from == 60) &&
            !(allPieces & ((1ULL<<61) | (1ULL<<62))) &&
            (board.getBlackRooks() & (1ULL<<63)) &&
            !isKingInCheck(board, false) &&
            !isSquareAttacked(board, 61, true) &&
            !isSquareAttacked(board, 62, true)) {
            moves.push_back("e8-g8 (Castle Kingside)");
        }
        if (board.canCastleBQ() && (from == 60) &&
            !(allPieces & ((1ULL<<57)|(1ULL<<58)|(1ULL<<59))) &&
            (board.getBlackRooks() & (1ULL<<56)) &&
            !isKingInCheck(board, false) &&
            !isSquareAttacked(board, 59, true) &&
            !isSquareAttacked(board, 58, true)) {
            moves.push_back("e8-c8 (Castle Queenside)");
        }
    }

    return moves;
}

std::vector<std::string> MoveGenerator::generateAllMoves(const Board& board, bool isWhite) const {
    std::vector<std::string> all;
    auto append = [&all](const std::vector<std::string>& mv) {
        all.insert(all.end(), mv.begin(), mv.end());
    };
    append(generatePawnMoves(board, isWhite));
    append(generateKnightMoves(board, isWhite));
    append(generateBishopMoves(board, isWhite));
    append(generateRookMoves(board, isWhite));
    append(generateQueenMoves(board, isWhite));
    append(generateKingMoves(board, isWhite));
    return all;
}

std::vector<std::string> MoveGenerator::generateLegalMoves(const Board& board, bool isWhite) const {
    auto pseudo = generateAllMoves(board, isWhite);
    std::vector<std::string> legal;
    for (const auto& mv : pseudo) {
        if (board.isMoveLegal(mv))
            legal.push_back(mv);
    }
    return legal;
}

bool MoveGenerator::isSquareAttacked(const Board& board, int square, bool byWhite) const {
    uint64_t occ = board.getWhitePieces() | board.getBlackPieces();
    uint64_t mask = 1ULL << square;

    if (byWhite) {
        uint64_t pawns = board.getWhitePawns();
        if (((pawns << 7) & 0x7F7F7F7F7F7F7F7FULL) & mask) return true;
        if (((pawns << 9) & 0xFEFEFEFEFEFEFEFEULL) & mask) return true;
    } else {
        uint64_t pawns = board.getBlackPawns();
        if (((pawns >> 7) & 0xFEFEFEFEFEFEFEFEULL) & mask) return true;
        if (((pawns >> 9) & 0x7F7F7F7F7F7F7F7FULL) & mask) return true;
    }

    uint64_t knights = byWhite ? board.getWhiteKnights() : board.getBlackKnights();
    const int kOffsets[8][2] = {{1,2},{2,1},{-1,2},{-2,1},{1,-2},{2,-1},{-1,-2},{-2,-1}};
    while (knights) {
        int from = popLSBIndex(knights);
        int fx = from % 8, fy = from / 8;
        for (auto &o : kOffsets) {
            int tx = fx + o[0], ty = fy + o[1];
            if (tx < 0 || tx > 7 || ty < 0 || ty > 7) continue;
            if (ty * 8 + tx == square) return true;
        }
    }

    uint64_t bishops = (byWhite ? board.getWhiteBishops() : board.getBlackBishops()) |
                       (byWhite ? board.getWhiteQueens() : board.getBlackQueens());
    while (bishops) {
        int from = popLSBIndex(bishops);
        if (Magic::getBishopAttacks(from, occ) & mask) return true;
    }

    uint64_t rooks = (byWhite ? board.getWhiteRooks() : board.getBlackRooks()) |
                     (byWhite ? board.getWhiteQueens() : board.getBlackQueens());
    while (rooks) {
        int from = popLSBIndex(rooks);
        if (Magic::getRookAttacks(from, occ) & mask) return true;
    }

    uint64_t king = byWhite ? board.getWhiteKing() : board.getBlackKing();
    if (king) {
        int from = lsbIndex(king);
        int fx = from % 8, fy = from / 8;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                int tx = fx + dx, ty = fy + dy;
                if (tx < 0 || tx > 7 || ty < 0 || ty > 7) continue;
                if (ty * 8 + tx == square) return true;
            }
        }
    }

    return false;
}

bool MoveGenerator::isKingInCheck(const Board& board, bool white) const {
    uint64_t king = white ? board.getWhiteKing() : board.getBlackKing();
    if (!king) return false;
    int sq = lsbIndex(king);
    return isSquareAttacked(board, sq, !white);
}


