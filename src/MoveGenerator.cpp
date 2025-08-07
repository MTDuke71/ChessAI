#include "MoveGenerator.h"
#include "BitUtils.h"
#include "Board.h"
#include "Magic.h"
#include "MoveEncoding.h"
#include <vector>
#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <cstdint>
#endif
#include <array>
#include <iostream> // For printing moves
#include <random>
#include <string>

namespace {
std::array<uint64_t, 64> knightAttackTable{};
std::array<uint64_t, 64> kingAttackTable{};
bool leaperTablesInitialized = false;

void initLeaperTables() {
  if (leaperTablesInitialized)
    return;
  const int knightOffsets[8][2] = {{1, 2},  {2, 1},  {-1, 2}, {-2, 1},
                                   {1, -2}, {2, -1}, {-1, -2}, {-2, -1}};
  for (int sq = 0; sq < 64; ++sq) {
    int r = sq / 8, f = sq % 8;
    uint64_t nMoves = 0ULL, kMoves = 0ULL;
    for (auto &o : knightOffsets) {
      int tr = r + o[1];
      int tf = f + o[0];
      if (tr >= 0 && tr < 8 && tf >= 0 && tf < 8)
        nMoves |= 1ULL << (tr * 8 + tf);
    }
    for (int dr = -1; dr <= 1; ++dr) {
      for (int df = -1; df <= 1; ++df) {
        if (dr == 0 && df == 0)
          continue;
        int tr = r + dr, tf = f + df;
        if (tr >= 0 && tr < 8 && tf >= 0 && tf < 8)
          kMoves |= 1ULL << (tr * 8 + tf);
      }
    }
    knightAttackTable[sq] = nMoves;
    kingAttackTable[sq] = kMoves;
  }
  leaperTablesInitialized = true;
}
} // namespace

MoveGenerator::MoveGenerator() {
  Magic::init();
  initLeaperTables();
}

std::vector<uint16_t> MoveGenerator::generatePawnMoves(const Board &board,
                                                       bool isWhite) const {
  std::vector<uint16_t> moves;
  uint64_t pawns = isWhite ? board.getWhitePawns() : board.getBlackPawns();
  uint64_t ownPieces =
      isWhite ? board.getWhitePieces() : board.getBlackPieces();
  uint64_t opponentPieces =
      isWhite ? board.getBlackPieces() : board.getWhitePieces();
  uint64_t emptySquares = ~(board.getWhitePieces() | board.getBlackPieces());

  const uint64_t whiteStartRank = 0x000000000000FF00ULL;
  const uint64_t blackStartRank = 0x00FF000000000000ULL;
  const uint64_t whitePromRank = 0xFF00000000000000ULL;
  const uint64_t blackPromRank = 0x00000000000000FFULL;

  if (isWhite) {
    // Single pushes
    uint64_t one = (pawns << 8) & emptySquares;
    for (uint64_t targets = one; targets; targets &= targets - 1) {
      int to = lsbIndex(targets);
      int from = to - 8;
      if ((1ULL << to) & whitePromRank) {
        for (char p : {'q', 'r', 'b', 'n'})
          moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + p));
      } else {
        moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
      }
    }

    // Double pushes from starting rank
    uint64_t two = (((pawns & whiteStartRank) << 8) & emptySquares) << 8;
    two &= emptySquares;
    for (uint64_t targets = two; targets; targets &= targets - 1) {
      int to = lsbIndex(targets);
      int from = to - 16;
      moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
    }

    // Captures
    uint64_t left = (pawns << 9) & opponentPieces & 0xFEFEFEFEFEFEFEFEULL;
    for (uint64_t targets = left; targets; targets &= targets - 1) {
      int to = lsbIndex(targets);
      int from = to - 9;
      if ((1ULL << to) & whitePromRank) {
        for (char p : {'q', 'r', 'b', 'n'})
          moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + p));
      } else {
        moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
      }
    }

    uint64_t right = (pawns << 7) & opponentPieces & 0x7F7F7F7F7F7F7F7FULL;
    for (uint64_t targets = right; targets; targets &= targets - 1) {
      int to = lsbIndex(targets);
      int from = to - 7;
      if ((1ULL << to) & whitePromRank) {
        for (char p : {'q', 'r', 'b', 'n'})
          moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + p));
      } else {
        moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
      }
    }
  } else {
    // Single pushes
    uint64_t one = (pawns >> 8) & emptySquares;
    for (uint64_t targets = one; targets; targets &= targets - 1) {
      int to = lsbIndex(targets);
      int from = to + 8;
      if ((1ULL << to) & blackPromRank) {
        for (char p : {'q', 'r', 'b', 'n'})
          moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + p));
      } else {
        moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
      }
    }

    // Double pushes from starting rank
    uint64_t two = (((pawns & blackStartRank) >> 8) & emptySquares) >> 8;
    two &= emptySquares;
    for (uint64_t targets = two; targets; targets &= targets - 1) {
      int to = lsbIndex(targets);
      int from = to + 16;
      moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
    }

    // Captures
    uint64_t left = (pawns >> 7) & opponentPieces & 0xFEFEFEFEFEFEFEFEULL;
    for (uint64_t targets = left; targets; targets &= targets - 1) {
      int to = lsbIndex(targets);
      int from = to + 7;
      if ((1ULL << to) & blackPromRank) {
        for (char p : {'q', 'r', 'b', 'n'})
          moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + p));
      } else {
        moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
      }
    }

    uint64_t right = (pawns >> 9) & opponentPieces & 0x7F7F7F7F7F7F7F7FULL;
    for (uint64_t targets = right; targets; targets &= targets - 1) {
      int to = lsbIndex(targets);
      int from = to + 9;
      if ((1ULL << to) & blackPromRank) {
        for (char p : {'q', 'r', 'b', 'n'})
          moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to) + p));
      } else {
        moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
      }
    }
  }

  // En passant
  if (board.getEnPassantSquare() != -1) {
    uint64_t epSquare = 1ULL << board.getEnPassantSquare();
    uint64_t fromMask;
    if (isWhite) {
      fromMask = ((epSquare >> 9) & pawns & 0x7F7F7F7F7F7F7F7FULL) |
                 ((epSquare >> 7) & pawns & 0xFEFEFEFEFEFEFEFEULL);
    } else {
      // Mirror the masks used for white so file wrapping is handled correctly
      // for black pawns capturing en passant from either side.
      fromMask = ((epSquare << 7) & pawns & 0x7F7F7F7F7F7F7F7FULL) |
                 ((epSquare << 9) & pawns & 0xFEFEFEFEFEFEFEFEULL);
    }

    for (uint64_t mask = fromMask; mask; mask &= mask - 1) {
      int from = lsbIndex(mask);
      int to = board.getEnPassantSquare();
      moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
    }
  }

  return moves;
}

void MoveGenerator::addMoves(std::vector<uint16_t> &moves, uint64_t pawns,
                             uint64_t moveBoard, int shift) const {
  for (int from = 0; moveBoard; moveBoard &= moveBoard - 1) {
    int to = lsbIndex(moveBoard);
    from = to - shift; // Calculate starting square
    moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
  }
}

std::vector<uint16_t>
MoveGenerator::generateKnightMoves(const Board &board, bool isWhite) const {
  std::vector<uint16_t> moves;
  uint64_t knights =
      isWhite ? board.getWhiteKnights() : board.getBlackKnights();
  uint64_t ownPieces =
      isWhite ? board.getWhitePieces() : board.getBlackPieces();
  while (knights) {
    int from = popLSBIndex(knights);
    uint64_t attacks = knightAttackTable[from] & ~ownPieces;
    for (uint64_t m = attacks; m; m &= m - 1) {
      int to = lsbIndex(m);
      moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
    }
  }

  return moves;
}

std::vector<uint16_t> MoveGenerator::generateRookMoves(const Board &board,
                                                       bool isWhite) const {
  std::vector<uint16_t> moves;
  uint64_t rooks = isWhite ? board.getWhiteRooks() : board.getBlackRooks();
  uint64_t ownPieces =
      isWhite ? board.getWhitePieces() : board.getBlackPieces();
  uint64_t occupancy = board.getWhitePieces() | board.getBlackPieces();
  while (rooks) {
    int from = popLSBIndex(rooks);
    uint64_t attacks = Magic::getRookAttacks(from, occupancy) & ~ownPieces;
    for (uint64_t m = attacks; m; m &= m - 1) {
      int to = lsbIndex(m);
      moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
    }
  }
  return moves;
}

std::vector<uint16_t>
MoveGenerator::generateBishopMoves(const Board &board, bool isWhite) const {
  std::vector<uint16_t> moves;
  uint64_t bishops =
      isWhite ? board.getWhiteBishops() : board.getBlackBishops();
  uint64_t ownPieces =
      isWhite ? board.getWhitePieces() : board.getBlackPieces();
  uint64_t occupancy = board.getWhitePieces() | board.getBlackPieces();
  while (bishops) {
    int from = popLSBIndex(bishops);
    uint64_t attacks = Magic::getBishopAttacks(from, occupancy) & ~ownPieces;
    for (uint64_t m = attacks; m; m &= m - 1) {
      int to = lsbIndex(m);
      moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
    }
  }
  return moves;
}

std::vector<uint16_t> MoveGenerator::generateQueenMoves(const Board &board,
                                                        bool isWhite) const {
  std::vector<uint16_t> moves;
  uint64_t queens = isWhite ? board.getWhiteQueens() : board.getBlackQueens();
  uint64_t ownPieces =
      isWhite ? board.getWhitePieces() : board.getBlackPieces();
  uint64_t occupancy = board.getWhitePieces() | board.getBlackPieces();
  while (queens) {
    int from = popLSBIndex(queens);
    uint64_t attacks = (Magic::getRookAttacks(from, occupancy) |
                        Magic::getBishopAttacks(from, occupancy)) &
                       ~ownPieces;
    for (uint64_t m = attacks; m; m &= m - 1) {
      int to = lsbIndex(m);
      moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
    }
  }
  return moves;
}

std::vector<uint16_t> MoveGenerator::generateKingMoves(const Board &board,
                                                       bool isWhite) const {
  std::vector<uint16_t> moves;
  uint64_t king = isWhite ? board.getWhiteKing() : board.getBlackKing();
  if (!king)
    return moves;
  uint64_t ownPieces =
      isWhite ? board.getWhitePieces() : board.getBlackPieces();
  int from = lsbIndex(king);
  uint64_t attacks = kingAttackTable[from] & ~ownPieces;
  for (uint64_t m = attacks; m; m &= m - 1) {
    int to = lsbIndex(m);
    moves.push_back(encodeMove(indexToAlgebraic(from) + "-" + indexToAlgebraic(to)));
  }

  uint64_t allPieces = board.getWhitePieces() | board.getBlackPieces();
  if (isWhite) {
    if (board.canCastleWK() && (from == 4) &&
        !(allPieces & ((1ULL << 5) | (1ULL << 6))) &&
        (board.getWhiteRooks() & (1ULL << 7)) && !isKingInCheck(board, true) &&
        !isSquareAttacked(board, 5, false) &&
        !isSquareAttacked(board, 6, false)) {
      moves.push_back(encodeMove("e1-g1"));
    }
    if (board.canCastleWQ() && (from == 4) &&
        !(allPieces & ((1ULL << 1) | (1ULL << 2) | (1ULL << 3))) &&
        (board.getWhiteRooks() & (1ULL << 0)) && !isKingInCheck(board, true) &&
        !isSquareAttacked(board, 3, false) &&
        !isSquareAttacked(board, 2, false)) {
      moves.push_back(encodeMove("e1-c1"));
    }
  } else {
    if (board.canCastleBK() && (from == 60) &&
        !(allPieces & ((1ULL << 61) | (1ULL << 62))) &&
        (board.getBlackRooks() & (1ULL << 63)) &&
        !isKingInCheck(board, false) && !isSquareAttacked(board, 61, true) &&
        !isSquareAttacked(board, 62, true)) {
      moves.push_back(encodeMove("e8-g8"));
    }
    if (board.canCastleBQ() && (from == 60) &&
        !(allPieces & ((1ULL << 57) | (1ULL << 58) | (1ULL << 59))) &&
        (board.getBlackRooks() & (1ULL << 56)) &&
        !isKingInCheck(board, false) && !isSquareAttacked(board, 59, true) &&
        !isSquareAttacked(board, 58, true)) {
      moves.push_back(encodeMove("e8-c8"));
    }
  }

  return moves;
}

std::vector<uint16_t> MoveGenerator::generateAllMoves(const Board &board,
                                                      bool isWhite) const {
  std::vector<uint16_t> all;
  auto append = [&all](const std::vector<uint16_t> &mv) {
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

std::vector<uint16_t> MoveGenerator::generateLegalMoves(const Board &board,
                                                        bool isWhite) const {
  auto pseudo = generateAllMoves(board, isWhite);
  std::vector<uint16_t> legal;
  for (auto mv : pseudo) {
    if (board.isMoveLegal(mv))
      legal.push_back(mv);
  }
  return legal;
}

bool MoveGenerator::isSquareAttacked(const Board &board, int square,
                                     bool byWhite) const {
  uint64_t occ = board.getWhitePieces() | board.getBlackPieces();
  uint64_t mask = 1ULL << square;

  if (byWhite) {
    uint64_t pawns = board.getWhitePawns();
    if (((pawns << 7) & 0x7F7F7F7F7F7F7F7FULL) & mask)
      return true;
    if (((pawns << 9) & 0xFEFEFEFEFEFEFEFEULL) & mask)
      return true;
  } else {
    uint64_t pawns = board.getBlackPawns();
    if (((pawns >> 7) & 0xFEFEFEFEFEFEFEFEULL) & mask)
      return true;
    if (((pawns >> 9) & 0x7F7F7F7F7F7F7F7FULL) & mask)
      return true;
  }

  uint64_t knights =
      byWhite ? board.getWhiteKnights() : board.getBlackKnights();
  while (knights) {
    int from = popLSBIndex(knights);
    if (knightAttackTable[from] & mask)
      return true;
  }

  uint64_t bishops =
      (byWhite ? board.getWhiteBishops() : board.getBlackBishops()) |
      (byWhite ? board.getWhiteQueens() : board.getBlackQueens());
  while (bishops) {
    int from = popLSBIndex(bishops);
    if (Magic::getBishopAttacks(from, occ) & mask)
      return true;
  }

  uint64_t rooks = (byWhite ? board.getWhiteRooks() : board.getBlackRooks()) |
                   (byWhite ? board.getWhiteQueens() : board.getBlackQueens());
  while (rooks) {
    int from = popLSBIndex(rooks);
    if (Magic::getRookAttacks(from, occ) & mask)
      return true;
  }

  uint64_t king = byWhite ? board.getWhiteKing() : board.getBlackKing();
  if (king) {
    int from = lsbIndex(king);
    if (kingAttackTable[from] & mask)
      return true;
  }

  return false;
}

bool MoveGenerator::isKingInCheck(const Board &board, bool white) const {
  uint64_t king = white ? board.getWhiteKing() : board.getBlackKing();
  if (!king)
    return false;
  int sq = lsbIndex(king);
  return isSquareAttacked(board, sq, !white);
}
