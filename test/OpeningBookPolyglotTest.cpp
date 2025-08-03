#define private public
#include "OpeningBook.h"
#undef private
#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

struct TestVector {
    std::string fen;
    uint64_t key;
};

void testPolyglotKeys() {
    std::vector<TestVector> vectors = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 0x463b96181691fc9cULL},
        {"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1", 0x823c9b50fd114196ULL},
        {"rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2", 0x0756b94461c50fb0ULL},
        {"rnbqkbnr/ppp1pppp/8/3pP3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2", 0x662fafb965db29d4ULL},
        {"rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3", 0x22a48b5a8e47ff78ULL},
        {"rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR b kq - 0 3", 0x652a607ca3f242c1ULL},
        {"rnbq1bnr/ppp1pkpp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR w - - 0 4", 0x00fdd303c946bdd9ULL},
        {"rnbqkbnr/p1pppppp/8/8/PpP4P/8/1P1PPPP1/RNBQKBNR b KQkq c3 0 3", 0x3c8123ea7b067637ULL},
        {"rnbqkbnr/p1pppppp/8/8/P6P/R1p5/1P1PPPP1/1NBQKBNR b Kkq - 0 4", 0x5c3f9b829b279560ULL}
    };

    for (const auto& tv : vectors) {
        Board board;
        bool ok = board.loadFEN(tv.fen);
        assert(ok);
        uint64_t hash = OpeningBook::polyglotHash(board);
        std::cout << "FEN: " << tv.fen << "\n";
        std::cout << "Expected: 0x" << std::hex << tv.key << ", Got: 0x" << hash << std::dec << "\n";
        assert(hash == tv.key);
    }
    std::cout << "[✔] Polyglot hash vectors validated\n";
}

int main() {
    testPolyglotKeys();
    std::cout << "All opening book tests passed\n";
    return 0;
}

