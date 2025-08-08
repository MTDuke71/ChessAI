#include "Board.h"
#include "Magic.h"
#include "MoveEncoding.h"
#include "MoveGenerator.h"
#include "Zobrist.h"
#include <array>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <sstream>

namespace {
const int directions[8][2] = {{1, 0}, {-1, 0}, {0, 1},  {0, -1},
                              {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

uint64_t knightAttacks(int sq) {
  uint64_t p = 1ULL << sq;
  uint64_t l1 = (p >> 1) & 0x7f7f7f7f7f7f7f7fULL;
  uint64_t l2 = (p >> 2) & 0x3f3f3f3f3f3f3f3fULL;
  uint64_t r1 = (p << 1) & 0xfefefefefefefefeULL;
  uint64_t r2 = (p << 2) & 0xfcfcfcfcfcfcfcfcULL;
  uint64_t h1 = l1 | r1;
  uint64_t h2 = l2 | r2;
  return (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
}

uint64_t kingAttacks(int sq) {
  uint64_t p = 1ULL << sq;
  uint64_t attacks = (p << 8) | (p >> 8);
  uint64_t lr =
      ((p << 1) & 0xfefefefefefefefeULL) | ((p >> 1) & 0x7f7f7f7f7f7f7f7fULL);
  attacks |= lr;
  attacks |= (lr << 8) | (lr >> 8);
  return attacks;
}

const std::array<int, 256 * 256> squareIndexLookup = [] {
  std::array<int, 256 * 256> arr{};
  arr.fill(-1);
  for (int i = 0; i < 64; ++i) {
    char file = 'a' + (i % 8);
    char rank = '1' + (i / 8);
    unsigned key = (static_cast<unsigned char>(file) << 8) |
                   static_cast<unsigned char>(rank);
    arr[key] = i;
  }
  return arr;
}();
} // namespace

//------------------------------------------------------------------------------
// Default constructor initializes the board to the standard starting position.
//------------------------------------------------------------------------------
Board::Board() {
  loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

//------------------------------------------------------------------------------
// Reset all bitboards and state information to represent an empty board.
//------------------------------------------------------------------------------
void Board::clearBoard() {
  whitePawns = whiteKnights = whiteBishops = whiteRooks = whiteQueens =
      whiteKing = 0;
  blackPawns = blackKnights = blackBishops = blackRooks = blackQueens =
      blackKing = 0;
  enPassantSquare = -1;
  whiteToMove = true;
  castleWK = castleWQ = castleBK = castleBQ = false;
  halfmoveClock = 0;
  fullmoveNumber = 1;
  repetitionTable.clear();
  attackMaps[0] = attackMaps[1] = 0;
  squareAttacks.fill(0);
}

uint64_t Board::computeAttacks(int sq) const {
  uint64_t occ = getWhitePieces() | getBlackPieces();
  uint64_t mask = 1ULL << sq;
  if (whitePawns & mask)
    return ((mask << 7) & 0x7F7F7F7F7F7F7F7FULL) |
           ((mask << 9) & 0xFEFEFEFEFEFEFEFEULL);
  if (blackPawns & mask)
    return ((mask >> 7) & 0xFEFEFEFEFEFEFEFEULL) |
           ((mask >> 9) & 0x7F7F7F7F7F7F7F7FULL);

  // Knight moves
  if ((whiteKnights | blackKnights) & mask) {
    const int offsets[8][2] = {{1, 2},  {2, 1},  {-1, 2},  {-2, 1},
                               {1, -2}, {2, -1}, {-1, -2}, {-2, -1}};
    int r = sq / 8, f = sq % 8;
    uint64_t attacks = 0ULL;
    for (auto &o : offsets) {
      int tr = r + o[1], tf = f + o[0];
      if (tr >= 0 && tr < 8 && tf >= 0 && tf < 8)
        attacks |= 1ULL << (tr * 8 + tf);
    }
    return attacks;
  }

  // Bishop
  if ((whiteBishops | blackBishops) & mask)
    return Magic::getBishopAttacks(sq, occ);

  // Rook
  if ((whiteRooks | blackRooks) & mask)
    return Magic::getRookAttacks(sq, occ);

  // Queen
  if ((whiteQueens | blackQueens) & mask)
    return Magic::getBishopAttacks(sq, occ) | Magic::getRookAttacks(sq, occ);

  // King
  if ((whiteKing | blackKing) & mask) {
    int r = sq / 8, f = sq % 8;
    uint64_t attacks = 0ULL;
    for (int dr = -1; dr <= 1; ++dr) {
      for (int df = -1; df <= 1; ++df) {
        if (dr == 0 && df == 0)
          continue;
        int tr = r + dr, tf = f + df;
        if (tr >= 0 && tr < 8 && tf >= 0 && tf < 8)
          attacks |= 1ULL << (tr * 8 + tf);
      }
    }
    return attacks;
  }

  return 0ULL;
}

void Board::updateLines(int sq) {
  if (sq < 0)
    return;
  uint64_t occ = getWhitePieces() | getBlackPieces();
  int r = sq / 8, f = sq % 8;
  for (auto &d : directions) {
    int tr = r + d[0];
    int tf = f + d[1];
    while (tr >= 0 && tr < 8 && tf >= 0 && tf < 8) {
      int idx = tr * 8 + tf;
      uint64_t bit = 1ULL << idx;
      if (occ & bit) {
        int side = (getWhitePieces() & bit) ? 0 : 1;
        attackMaps[side] &= ~squareAttacks[idx];
        squareAttacks[idx] = computeAttacks(idx);
        attackMaps[side] |= squareAttacks[idx];
        break;
      }
      tr += d[0];
      tf += d[1];
    }
  }
}

void Board::recalculateAttacks() {
  static bool magicInit = false;
  if (!magicInit) {
    Magic::init();
    magicInit = true;
  }
  attackMaps[0] = attackMaps[1] = 0;
  squareAttacks.fill(0);
  uint64_t occ = getWhitePieces() | getBlackPieces();
  for (int sq = 0; sq < 64; ++sq) {
    uint64_t bit = 1ULL << sq;
    if (occ & bit) {
      squareAttacks[sq] = computeAttacks(sq);
      int side = (getWhitePieces() & bit) ? 0 : 1;
      attackMaps[side] |= squareAttacks[sq];
    }
  }
}

//------------------------------------------------------------------------------
// Display the current board state in a simple ASCII diagram.
//------------------------------------------------------------------------------
void Board::printBoard() const {
  std::cout << "   a b c d e f g h\n";
  std::cout << "  +-----------------+\n";

  for (int rank = 7; rank >= 0; --rank) {
    std::cout << " " << rank + 1 << "|";

    for (int file = 0; file < 8; ++file) {
      uint64_t square = 1ULL << (rank * 8 + file);
      if (whitePawns & square)
        std::cout << " P";
      else if (whiteRooks & square)
        std::cout << " R";
      else if (whiteKnights & square)
        std::cout << " N";
      else if (whiteBishops & square)
        std::cout << " B";
      else if (whiteQueens & square)
        std::cout << " Q";
      else if (whiteKing & square)
        std::cout << " K";
      else if (blackPawns & square)
        std::cout << " p";
      else if (blackRooks & square)
        std::cout << " r";
      else if (blackKnights & square)
        std::cout << " n";
      else if (blackBishops & square)
        std::cout << " b";
      else if (blackQueens & square)
        std::cout << " q";
      else if (blackKing & square)
        std::cout << " k";
      else
        std::cout << " .";
    }
    std::cout << " | " << rank + 1 << "\n";
  }

  std::cout << "  +-----------------+\n";
  std::cout << "   a b c d e f g h\n\n";
}

//------------------------------------------------------------------------------
// Load a board position from a FEN string.
// Returns true on success, false if the string is malformed.
//------------------------------------------------------------------------------
bool Board::loadFEN(const std::string &fen) {
  clearBoard();
  std::stringstream ss(fen);
  std::string boardPart, active, castling, ep;
  int half = 0, full = 1;

  if (!(ss >> boardPart >> active >> castling >> ep))
    return false;
  ss >> half >> full;

  int rank = 7, file = 0;
  for (char c : boardPart) {
    if (c == '/') {
      --rank;
      file = 0;
      continue;
    }
    if (std::isdigit(c)) {
      file += c - '0';
      continue;
    }

    int index = rank * 8 + file;
    uint64_t bit = 1ULL << index;
    switch (c) {
    case 'P':
      whitePawns |= bit;
      break;
    case 'N':
      whiteKnights |= bit;
      break;
    case 'B':
      whiteBishops |= bit;
      break;
    case 'R':
      whiteRooks |= bit;
      break;
    case 'Q':
      whiteQueens |= bit;
      break;
    case 'K':
      whiteKing |= bit;
      break;
    case 'p':
      blackPawns |= bit;
      break;
    case 'n':
      blackKnights |= bit;
      break;
    case 'b':
      blackBishops |= bit;
      break;
    case 'r':
      blackRooks |= bit;
      break;
    case 'q':
      blackQueens |= bit;
      break;
    case 'k':
      blackKing |= bit;
      break;
    }
    ++file;
  }

  whiteToMove = (active == "w");
  castleWK = castling.find('K') != std::string::npos;
  castleWQ = castling.find('Q') != std::string::npos;
  castleBK = castling.find('k') != std::string::npos;
  castleBQ = castling.find('q') != std::string::npos;

  if (ep != "-") {
    int f = ep[0] - 'a';
    int r = ep[1] - '1';
    enPassantSquare = r * 8 + f;
  } else {
    enPassantSquare = -1;
  }

  halfmoveClock = half;
  fullmoveNumber = full;
  repetitionTable.clear();
  repetitionTable[Zobrist::hashBoard(*this)] = 1;
  recalculateAttacks();

  return true;
}

//------------------------------------------------------------------------------
// Convert the current board state into a FEN string.
//------------------------------------------------------------------------------
std::string Board::getFEN() const {
  std::string fen;
  for (int rank = 7; rank >= 0; --rank) {
    int empty = 0;
    for (int file = 0; file < 8; ++file) {
      int index = rank * 8 + file;
      uint64_t mask = 1ULL << index;
      char piece = 0;
      if (whitePawns & mask)
        piece = 'P';
      else if (whiteKnights & mask)
        piece = 'N';
      else if (whiteBishops & mask)
        piece = 'B';
      else if (whiteRooks & mask)
        piece = 'R';
      else if (whiteQueens & mask)
        piece = 'Q';
      else if (whiteKing & mask)
        piece = 'K';
      else if (blackPawns & mask)
        piece = 'p';
      else if (blackKnights & mask)
        piece = 'n';
      else if (blackBishops & mask)
        piece = 'b';
      else if (blackRooks & mask)
        piece = 'r';
      else if (blackQueens & mask)
        piece = 'q';
      else if (blackKing & mask)
        piece = 'k';
      if (piece) {
        if (empty) {
          fen += std::to_string(empty);
          empty = 0;
        }
        fen += piece;
      } else {
        ++empty;
      }
    }
    if (empty)
      fen += std::to_string(empty);
    if (rank > 0)
      fen += '/';
  }
  fen += whiteToMove ? " w " : " b ";
  std::string castling;
  if (castleWK)
    castling += 'K';
  if (castleWQ)
    castling += 'Q';
  if (castleBK)
    castling += 'k';
  if (castleBQ)
    castling += 'q';
  if (castling.empty())
    castling = "-";
  fen += castling + " ";
  if (enPassantSquare >= 0) {
    int f = enPassantSquare % 8;
    int r = enPassantSquare / 8;
    fen += std::string{static_cast<char>('a' + f), static_cast<char>('1' + r)};
  } else {
    fen += "-";
  }
  fen += " " + std::to_string(halfmoveClock) + " " +
         std::to_string(fullmoveNumber);
  return fen;
}

int algebraicToIndex(const std::string &sq) {
  if (sq.size() != 2)
    return -1;
  unsigned key = (static_cast<unsigned char>(sq[0]) << 8) |
                 static_cast<unsigned char>(sq[1]);
  return squareIndexLookup[key];
}

Board::Color Board::pieceColorAt(int index) const {
  uint64_t mask = 1ULL << index;
  if (getWhitePieces() & mask)
    return Color::White;
  if (getBlackPieces() & mask)
    return Color::Black;
  return Color::None;
}

//------------------------------------------------------------------------------
// Check whether a move in algebraic format (e2-e4) is legal.
//------------------------------------------------------------------------------
bool Board::isMoveLegal(const std::string &move) const {
  return isMoveLegal(encodeMove(move));
}

bool Board::isMoveLegal(uint16_t move) const {
  MoveGenerator gen;
  int from = moveFrom(move);
  int to = moveTo(move);
  int special = moveSpecial(move);
  bool isWhite = whiteToMove;
  uint64_t fromMask = 1ULL << from;
  uint64_t toMask = 1ULL << to;
  uint64_t own = isWhite ? getWhitePieces() : getBlackPieces();
  uint64_t opp = isWhite ? getBlackPieces() : getWhitePieces();
  uint64_t occ = own | opp;

  // Allow castling even though the destination square contains our rook.
  // Only a king may make such a move; otherwise moving onto a friendly piece
  // is always illegal.
  if (!(own & fromMask))
    return false;
  if (own & toMask) {
    bool kingAtFrom = (whiteKing | blackKing) & fromMask;
    if (!(special == 3 && kingAtFrom &&
          ((from == 4 && (to == 7 || to == 0)) ||
           (from == 60 && (to == 63 || to == 56)))))
      return false;
  }

  bool pseudo = false;

  if ((whitePawns & fromMask) || (blackPawns & fromMask)) {
    int dir = isWhite ? 8 : -8;
    bool promotion = (isWhite && to >= 56) || (!isWhite && to <= 7);
    if (to == from + dir && !(occ & toMask)) {
      pseudo = true;
    } else if (to == from + 2 * dir && !(occ & toMask) &&
               !(occ & (1ULL << (from + dir))) &&
               ((isWhite && from >= 8 && from < 16) ||
                (!isWhite && from >= 48 && from < 56))) {
      pseudo = true;
    } else if ((to == from + dir + 1 && from % 8 != 7) ||
               (to == from + dir - 1 && from % 8 != 0)) {
      if (opp & toMask)
        pseudo = true;
      else if (to == enPassantSquare && enPassantSquare != -1 &&
               !(occ & toMask))
        pseudo = true;
    }
    if (promotion) {
      if (special != 1)
        return false;
    } else if (special == 1) {
      return false;
    }
  } else if ((whiteKnights & fromMask) || (blackKnights & fromMask)) {
    if ((knightAttacks(from) & ~own & toMask))
      pseudo = true;
    if (special != 0)
      return false;
  } else if ((whiteBishops & fromMask) || (blackBishops & fromMask)) {
    if ((Magic::getBishopAttacks(from, occ) & ~own & toMask))
      pseudo = true;
    if (special != 0)
      return false;
  } else if ((whiteRooks & fromMask) || (blackRooks & fromMask)) {
    if ((Magic::getRookAttacks(from, occ) & ~own & toMask))
      pseudo = true;
    if (special != 0)
      return false;
  } else if ((whiteQueens & fromMask) || (blackQueens & fromMask)) {
    uint64_t attacks =
        (Magic::getBishopAttacks(from, occ) | Magic::getRookAttacks(from, occ));
    if ((attacks & ~own & toMask))
      pseudo = true;
    if (special != 0)
      return false;
  } else if ((whiteKing & fromMask) || (blackKing & fromMask)) {
    uint64_t attacks = kingAttacks(from) & ~own;
    if (attacks & toMask) {
      pseudo = true;
      if (special != 0)
        return false;
    } else if (special == 3) {
      if (isWhite) {
        if (from == 4 && to == 7 && castleWK &&
            !(occ & ((1ULL << 5) | (1ULL << 6))) &&
            !gen.isSquareAttacked(*this, 4, false) &&
            !gen.isSquareAttacked(*this, 5, false) &&
            !gen.isSquareAttacked(*this, 6, false))
          pseudo = true;
        else if (from == 4 && to == 0 && castleWQ &&
                 !(occ & ((1ULL << 1) | (1ULL << 2) | (1ULL << 3))) &&
                 !gen.isSquareAttacked(*this, 4, false) &&
                 !gen.isSquareAttacked(*this, 3, false) &&
                 !gen.isSquareAttacked(*this, 2, false))
          pseudo = true;
      } else {
        if (from == 60 && to == 63 && castleBK &&
            !(occ & ((1ULL << 61) | (1ULL << 62))) &&
            !gen.isSquareAttacked(*this, 60, true) &&
            !gen.isSquareAttacked(*this, 61, true) &&
            !gen.isSquareAttacked(*this, 62, true))
          pseudo = true;
        else if (from == 60 && to == 56 && castleBQ &&
                 !(occ & ((1ULL << 57) | (1ULL << 58) | (1ULL << 59))) &&
                 !gen.isSquareAttacked(*this, 60, true) &&
                 !gen.isSquareAttacked(*this, 59, true) &&
                 !gen.isSquareAttacked(*this, 58, true))
          pseudo = true;
      }
    }
  } else {
    return false;
  }

  if (!pseudo)
    return false;

  Board copy = *this;
  copy.applyMove(move);
  return !gen.isKingInCheck(copy, !copy.isWhiteToMove());
}

//------------------------------------------------------------------------------
// Validate and apply a move, printing an error if the move is illegal.
//------------------------------------------------------------------------------
void Board::makeMove(const std::string &move) { makeMove(encodeMove(move, whiteToMove)); }

void Board::makeMove(uint16_t move) {
  if (!isMoveLegal(move)) {
    std::cerr << "Illegal move attempted: " << decodeMove(move) << "\n";
    return;
  }
  applyMove(move);
}

void Board::makeMove(const std::string &move, MoveState &state) {
  makeMove(encodeMove(move, whiteToMove), state);
}

void Board::makeMove(uint16_t move, MoveState &state) {
  state.whitePawns = whitePawns;
  state.whiteKnights = whiteKnights;
  state.whiteBishops = whiteBishops;
  state.whiteRooks = whiteRooks;
  state.whiteQueens = whiteQueens;
  state.whiteKing = whiteKing;
  state.blackPawns = blackPawns;
  state.blackKnights = blackKnights;
  state.blackBishops = blackBishops;
  state.blackRooks = blackRooks;
  state.blackQueens = blackQueens;
  state.blackKing = blackKing;
  state.enPassantSquare = enPassantSquare;
  state.whiteToMove = whiteToMove;
  state.castleWK = castleWK;
  state.castleWQ = castleWQ;
  state.castleBK = castleBK;
  state.castleBQ = castleBQ;
  state.halfmoveClock = halfmoveClock;
  state.fullmoveNumber = fullmoveNumber;
  state.whiteAttacks = attackMaps[0];
  state.blackAttacks = attackMaps[1];
  state.squareAttacks = squareAttacks;

  applyMove(move);
  state.zobristKey = Zobrist::hashBoard(*this);
}

void Board::unmakeMove(const MoveState &state) {
  // Decrement count for the current position before restoring the previous one
  uint64_t currentKey = Zobrist::hashBoard(*this);
  auto it = repetitionTable.find(currentKey);
  if (it != repetitionTable.end()) {
    if (--it->second == 0)
      repetitionTable.erase(it);
  }

  whitePawns = state.whitePawns;
  whiteKnights = state.whiteKnights;
  whiteBishops = state.whiteBishops;
  whiteRooks = state.whiteRooks;
  whiteQueens = state.whiteQueens;
  whiteKing = state.whiteKing;
  blackPawns = state.blackPawns;
  blackKnights = state.blackKnights;
  blackBishops = state.blackBishops;
  blackRooks = state.blackRooks;
  blackQueens = state.blackQueens;
  blackKing = state.blackKing;
  enPassantSquare = state.enPassantSquare;
  whiteToMove = state.whiteToMove;
  castleWK = state.castleWK;
  castleWQ = state.castleWQ;
  castleBK = state.castleBK;
  castleBQ = state.castleBQ;
  halfmoveClock = state.halfmoveClock;
  fullmoveNumber = state.fullmoveNumber;
  attackMaps[0] = state.whiteAttacks;
  attackMaps[1] = state.blackAttacks;
  squareAttacks = state.squareAttacks;
}

//------------------------------------------------------------------------------
// Apply a (pre-validated) move to the board and update game state.
// The move string is expected in the format "e2-e4" with an optional promotion
// piece (e.g., "e7-e8q").
//------------------------------------------------------------------------------
void Board::applyMove(const std::string &move) { applyMove(encodeMove(move)); }

void Board::applyMove(uint16_t move) {
  int from = moveFrom(move);
  int to = moveTo(move);
  if (from < 0 || to < 0)
    return;

  char promoChar = 0;
  if (moveSpecial(move) == 1) {
    switch (movePromotion(move)) {
    case 0:
      promoChar = 'n';
      break;
    case 1:
      promoChar = 'b';
      break;
    case 2:
      promoChar = 'r';
      break;
    case 3:
      promoChar = 'q';
      break;
    }
  }
  // Determine move characteristics
  uint64_t fromMask = 1ULL << from;
  uint64_t toMask = 1ULL << to;
  bool capture = ((getWhitePieces() | getBlackPieces()) & toMask);
  bool pawnMove = (whitePawns & fromMask) || (blackPawns & fromMask);

  int prevEnPassant = enPassantSquare;
  bool enPassantCapture = pawnMove && to == prevEnPassant && !capture;
  if (enPassantCapture) {
    uint64_t capMask = whiteToMove ? (1ULL << (to - 8)) : (1ULL << (to + 8));
    if (whiteToMove)
      blackPawns &= ~capMask;
    else
      whitePawns &= ~capMask;
    capture = true;
  }

  int side = whiteToMove ? 0 : 1;
  int oppSide = 1 - side;
  int capturedSquare =
      enPassantCapture ? (whiteToMove ? to - 8 : to + 8) : (capture ? to : -1);
  attackMaps[side] &= ~squareAttacks[from];
  squareAttacks[from] = 0;
  if (capturedSquare >= 0) {
    attackMaps[oppSide] &= ~squareAttacks[capturedSquare];
    squareAttacks[capturedSquare] = 0;
  }

  if (toMask & whiteRooks) {
    if (to == 0)
      castleWQ = false;
    if (to == 7)
      castleWK = false;
  }
  if (toMask & blackRooks) {
    if (to == 56)
      castleBQ = false;
    if (to == 63)
      castleBK = false;
  }

  uint64_t mask = ~toMask;
  whitePawns &= mask;
  whiteKnights &= mask;
  whiteBishops &= mask;
  whiteRooks &= mask;
  whiteQueens &= mask;
  whiteKing &= mask;
  blackPawns &= mask;
  blackKnights &= mask;
  blackBishops &= mask;
  blackRooks &= mask;
  blackQueens &= mask;
  blackKing &= mask;

  auto movePiece = [&](uint64_t &bb) {
    if (bb & fromMask) {
      bb &= ~fromMask;
      bb |= toMask;
      return true;
    }
    return false;
  };

  bool movedWhiteKing = (whiteKing & fromMask);
  bool movedBlackKing = (blackKing & fromMask);
  bool movedWhiteRook = (whiteRooks & fromMask);
  bool movedBlackRook = (blackRooks & fromMask);

  if (!(movePiece(whitePawns) || movePiece(whiteKnights) ||
        movePiece(whiteBishops) || movePiece(whiteRooks) ||
        movePiece(whiteQueens) || movePiece(whiteKing) ||
        movePiece(blackPawns) || movePiece(blackKnights) ||
        movePiece(blackBishops) || movePiece(blackRooks) ||
        movePiece(blackQueens) || movePiece(blackKing))) {
    return;
  }

  if (movedWhiteKing) {
    castleWK = castleWQ = false;
    if (from == 4 && to == 6) {  // Kingside castling: e1-g1
      attackMaps[0] &= ~squareAttacks[7];
      squareAttacks[7] = 0;
      whiteRooks &= ~(1ULL << 7);
      whiteRooks |= (1ULL << 5);
      squareAttacks[5] = computeAttacks(5);
      attackMaps[0] |= squareAttacks[5];
      updateLines(7);
      updateLines(5);
    } else if (from == 4 && to == 2) {  // Queenside castling: e1-c1
      attackMaps[0] &= ~squareAttacks[0];
      squareAttacks[0] = 0;
      whiteRooks &= ~(1ULL << 0);
      whiteRooks |= (1ULL << 3);
      squareAttacks[3] = computeAttacks(3);
      attackMaps[0] |= squareAttacks[3];
      updateLines(0);
      updateLines(3);
    }
  }
  if (movedBlackKing) {
    castleBK = castleBQ = false;
    if (from == 60 && to == 62) {  // Kingside castling: e8-g8
      attackMaps[1] &= ~squareAttacks[63];
      squareAttacks[63] = 0;
      blackRooks &= ~(1ULL << 63);
      blackRooks |= (1ULL << 61);
      squareAttacks[61] = computeAttacks(61);
      attackMaps[1] |= squareAttacks[61];
      updateLines(63);
      updateLines(61);
    } else if (from == 60 && to == 58) {  // Queenside castling: e8-c8
      attackMaps[1] &= ~squareAttacks[56];
      squareAttacks[56] = 0;
      blackRooks &= ~(1ULL << 56);
      blackRooks |= (1ULL << 59);
      squareAttacks[59] = computeAttacks(59);
      attackMaps[1] |= squareAttacks[59];
      updateLines(56);
      updateLines(59);
    }
  }

  if (movedWhiteRook) {
    if (from == 0)
      castleWQ = false;
    if (from == 7)
      castleWK = false;
  }
  if (movedBlackRook) {
    if (from == 56)
      castleBQ = false;
    if (from == 63)
      castleBK = false;
  }

  if (promoChar && pawnMove) {
    if (whiteToMove) {
      whitePawns &= ~toMask;
      switch (promoChar) {
      case 'q':
        whiteQueens |= toMask;
        break;
      case 'r':
        whiteRooks |= toMask;
        break;
      case 'b':
        whiteBishops |= toMask;
        break;
      case 'n':
        whiteKnights |= toMask;
        break;
      }
    } else {
      blackPawns &= ~toMask;
      switch (promoChar) {
      case 'q':
        blackQueens |= toMask;
        break;
      case 'r':
        blackRooks |= toMask;
        break;
      case 'b':
        blackBishops |= toMask;
        break;
      case 'n':
        blackKnights |= toMask;
        break;
      }
    }
  }

  squareAttacks[to] = computeAttacks(to);
  attackMaps[side] |= squareAttacks[to];
  updateLines(from);
  updateLines(to);
  if (capturedSquare >= 0)
    updateLines(capturedSquare);

  if (pawnMove && std::abs(to - from) == 16) {
    int mid = (from + to) / 2;
    uint64_t adjMask = whiteToMove ? blackPawns : whitePawns;
    bool leftAdj = (to % 8 > 0) && (adjMask & (1ULL << (to - 1)));
    bool rightAdj = (to % 8 < 7) && (adjMask & (1ULL << (to + 1)));
    if (leftAdj || rightAdj)
      enPassantSquare = mid;
    else
      enPassantSquare = -1;
  } else {
    enPassantSquare = -1;
  }

  whiteToMove = !whiteToMove;
  if (pawnMove || capture)
    halfmoveClock = 0;
  else
    ++halfmoveClock;
  if (!whiteToMove)
    ++fullmoveNumber;

  uint64_t key = Zobrist::hashBoard(*this);
  repetitionTable[key]++;
}

//------------------------------------------------------------------------------
// Determine whether the current position has occurred three or more times.
//------------------------------------------------------------------------------
bool Board::isThreefoldRepetition() const {
  uint64_t key = Zobrist::hashBoard(*this);
  auto it = repetitionTable.find(key);
  return it != repetitionTable.end() && it->second >= 3;
}

//------------------------------------------------------------------------------
// Get how many times the current position has appeared in the game history.
//------------------------------------------------------------------------------
int Board::repetitionCount() const {
  uint64_t key = Zobrist::hashBoard(*this);
  auto it = repetitionTable.find(key);
  return it != repetitionTable.end() ? it->second : 0;
}

//------------------------------------------------------------------------------
// Check whether the side to move has no legal moves and is not in check.
//------------------------------------------------------------------------------
bool Board::isStalemate() const {
  MoveGenerator gen;
  if (gen.isKingInCheck(*this, whiteToMove))
    return false;
  auto moves = gen.generateLegalMoves(*this, whiteToMove);
  return moves.empty();
}

//------------------------------------------------------------------------------
// Check whether the side to move is in check and has no legal moves.
//------------------------------------------------------------------------------
bool Board::isCheckmate() const {
  MoveGenerator gen;
  if (!gen.isKingInCheck(*this, whiteToMove))
    return false;
  auto moves = gen.generateLegalMoves(*this, whiteToMove);
  return moves.empty();
}
