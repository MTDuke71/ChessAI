#include "OpeningBook.h"
#include "PolyglotRandom.h"
#include <fstream>
#include <iostream>

namespace {
uint64_t readU64(std::ifstream& f) {
    unsigned char b[8];
    if (!f.read(reinterpret_cast<char*>(b), 8)) return 0;
    return (uint64_t)b[0]<<56 | (uint64_t)b[1]<<48 | (uint64_t)b[2]<<40 |
           (uint64_t)b[3]<<32 | (uint64_t)b[4]<<24 | (uint64_t)b[5]<<16 |
           (uint64_t)b[6]<<8  | (uint64_t)b[7];
}
uint16_t readU16(std::ifstream& f) {
    unsigned char b[2];
    if (!f.read(reinterpret_cast<char*>(b), 2)) return 0;
    return (uint16_t)b[0]<<8 | (uint16_t)b[1];
}
uint32_t readU32(std::ifstream& f) {
    unsigned char b[4];
    if (!f.read(reinterpret_cast<char*>(b), 4)) return 0;
    return (uint32_t)b[0]<<24 | (uint32_t)b[1]<<16 | (uint32_t)b[2]<<8 |
           (uint32_t)b[3];
}
} // namespace

OpeningBook::OpeningBook(const std::string& file) {
    std::ifstream in(file, std::ios::binary);
    if (!in) {
        std::ifstream alt("../" + file, std::ios::binary);
        if (alt) in.swap(alt);
    }
    if (!in) return;
    while (in.peek()!=EOF) {
        uint64_t key = readU64(in);
        uint16_t move = readU16(in);
        uint16_t weight = readU16(in);
        uint32_t learn = readU32(in);
        if (!in) break;
        (void)learn;
        entries.emplace(key, Entry{move, weight, learn});
    }
}

uint64_t OpeningBook::polyglotHash(const Board& b) {
    uint64_t h = 0;
    auto addPieces = [&](uint64_t bb, int pieceIndex) {
        while (bb) {
            int sq;
#if defined(_MSC_VER)
            unsigned long idx; _BitScanForward64(&idx, bb); sq = static_cast<int>(idx); bb &= bb-1;
#else
            sq = __builtin_ctzll(bb); bb &= bb-1ULL;
#endif
            h ^= polyglotRandom[pieceIndex * 64 + sq];
        }
    };
    // Polyglot orders pieces by type with black then white: BP, WP, BN, WN, ...
    addPieces(b.getBlackPawns(),   0);
    addPieces(b.getWhitePawns(),   1);
    addPieces(b.getBlackKnights(), 2);
    addPieces(b.getWhiteKnights(), 3);
    addPieces(b.getBlackBishops(), 4);
    addPieces(b.getWhiteBishops(), 5);
    addPieces(b.getBlackRooks(),   6);
    addPieces(b.getWhiteRooks(),   7);
    addPieces(b.getBlackQueens(),  8);
    addPieces(b.getWhiteQueens(),  9);
    addPieces(b.getBlackKing(),    10);
    addPieces(b.getWhiteKing(),    11);
    if (b.canCastleWK()) h ^= polyglotRandom[768];
    if (b.canCastleWQ()) h ^= polyglotRandom[769];
    if (b.canCastleBK()) h ^= polyglotRandom[770];
    if (b.canCastleBQ()) h ^= polyglotRandom[771];
    int ep = b.getEnPassantSquare();
    if (ep != -1) {
        uint64_t epMask = 1ULL << ep;
        bool valid = false;
        if (b.isWhiteToMove()) {
            uint64_t wp = b.getWhitePawns();
            valid = ((epMask >> 9) & wp & 0xFEFEFEFEFEFEFEFEULL) ||
                    ((epMask >> 7) & wp & 0x7F7F7F7F7F7F7F7FULL);
        } else {
            uint64_t bp = b.getBlackPawns();
            valid = ((epMask << 7) & bp & 0xFEFEFEFEFEFEFEFEULL) ||
                    ((epMask << 9) & bp & 0x7F7F7F7F7F7F7F7FULL);
        }
        if (valid) h ^= polyglotRandom[772 + (ep % 8)];
    }
    if (b.isWhiteToMove()) h ^= polyglotRandom[780];
    return h;
}

std::string OpeningBook::decodeMove(uint16_t mv) {
    int to = mv & 0x3f;
    int from = (mv >> 6) & 0x3f;
    int promo = (mv >> 12) & 0x7;

    // Polyglot represents castling moves using the king's origin square
    // and the rook's destination square. Convert these to the standard
    // UCI king destination squares.
    if (from == 4 && to == 7)       to = 6;  // white short  e1h1 -> e1g1
    else if (from == 4 && to == 0)  to = 2;  // white long   e1a1 -> e1c1
    else if (from == 60 && to == 63) to = 62; // black short  e8h8 -> e8g8
    else if (from == 60 && to == 56) to = 58; // black long   e8a8 -> e8c8

    std::string s = indexToAlgebraic(from) + indexToAlgebraic(to);
    if (promo) {
        char p = 'q';
        if (promo == 1) p = 'n';
        else if (promo == 2) p = 'b';
        else if (promo == 3) p = 'r';
        else if (promo == 4) p = 'q';
        s += p;
    }
    return s;
}

std::optional<std::string> OpeningBook::getBookMove(const Board& board) const {
    uint64_t key = polyglotHash(board);
    auto range = entries.equal_range(key);
    if (range.first == range.second) return std::nullopt;
    auto best = range.first;
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second.weight > best->second.weight) best = it;
    }
    return decodeMove(best->second.move);
}

void OpeningBook::print(std::ostream& out) const {
    for (const auto& kv : entries) {
        out << std::hex << kv.first << std::dec << " "
            << decodeMove(kv.second.move) << " "
            << kv.second.weight << " "
            << kv.second.learn << '\n';
    }
}

