#include "MoveEncoding.h"
#include <cctype>

namespace {
int squareToIndex(const std::string& sq) {
    if (sq.size() < 2) return -1;
    int file = sq[0] - 'a';
    int rank = sq[1] - '1';
    return rank * 8 + file;
}

std::string indexToSquare(int idx) {
    std::string s;
    s.push_back('a' + (idx % 8));
    s.push_back('1' + (idx / 8));
    return s;
}
}

uint16_t encodeMove(const std::string& move) {
    auto dash = move.find('-');
    if (dash == std::string::npos) return 0;
    int from = squareToIndex(move.substr(0, 2));
    int to = squareToIndex(move.substr(dash + 1, 2));
    if (from < 0 || to < 0) return 0;
    uint16_t code = static_cast<uint16_t>((to & 0x3f) | ((from & 0x3f) << 6));
    int special = 0;
    if (move.size() > dash + 3) {
        special = 1;
        char promo = std::tolower(move.back());
        int promoBits = 0;
        switch (promo) {
            case 'n': promoBits = 0; break;
            case 'b': promoBits = 1; break;
            case 'r': promoBits = 2; break;
            case 'q': promoBits = 3; break;
            default: promoBits = 3; break;
        }
        code |= (promoBits & 0x3) << 12;
    } else if ((from == 4 && (to == 7 || to == 0)) ||
               (from == 60 && (to == 63 || to == 56))) {
        special = 3; // castling
    }
    code |= (special & 0x3) << 14;
    return code;
}

std::string decodeMove(uint16_t code) {
    int to = code & 0x3f;
    int from = (code >> 6) & 0x3f;
    int promo = (code >> 12) & 0x3;
    int special = (code >> 14) & 0x3;
    std::string move = indexToSquare(from) + "-" + indexToSquare(to);
    if (special == 1) {
        char pc = 'q';
        switch (promo) {
            case 0: pc = 'n'; break;
            case 1: pc = 'b'; break;
            case 2: pc = 'r'; break;
            case 3: pc = 'q'; break;
        }
        move += pc;
    }
    return move;
}
