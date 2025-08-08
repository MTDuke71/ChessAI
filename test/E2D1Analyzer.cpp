#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

// Focused analyzer for the e2-d1 bishop move discrepancy.
// It compares pseudo-legal vs legal move counts after e2-d1 and
// classifies which white replies are being filtered out after each black reply.

static std::string decode(uint16_t m) {
    int to = m & 0x3f;
    int from = (m >> 6) & 0x3f;
    int promo = (m >> 12) & 0x3;
    int special = (m >> 14) & 0x3;
    if (special == 3) { // castling
        if (to - from == 2) return "O-O";
        if (from - to == 2) return "O-O-O";
    }
    std::string s;
    s += char('a' + (from % 8));
    s += char('1' + (from / 8));
    s += '-';
    s += char('a' + (to % 8));
    s += char('1' + (to / 8));
    if (special == 1) { // promotion stored as knight=0,bishop=1,rook=2,queen=3
        static const char prom[4] = {'n','b','r','q'};
        s += prom[promo];
    }
    return s;
}

struct MissInfo { int count = 0; std::unordered_map<std::string,int> pieceMap; };

// Identify piece type at square (for white side only here)
static std::string whitePieceAt(const Board& b, int sq) {
    uint64_t mask = 1ULL << sq;
    if (b.getWhitePawns() & mask) return "P";
    if (b.getWhiteKnights() & mask) return "N";
    if (b.getWhiteBishops() & mask) return "B";
    if (b.getWhiteRooks() & mask) return "R";
    if (b.getWhiteQueens() & mask) return "Q";
    if (b.getWhiteKing() & mask) return "K";
    return ".";
}

int main() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board root; root.loadFEN(fen);

    // Apply e2-d1
    Board::MoveState stRoot;
    root.makeMove("e2-d1", stRoot);

    MoveGenerator gen;
    auto blackPseudo = gen.generateAllMoves(root, false);

    std::cout << "=== e2-d1 FOLLOW-UP ANALYZER ===\n";
    std::cout << "Black pseudo moves after e2-d1: " << blackPseudo.size() << "\n";

    uint64_t totalLegalWhite = 0;
    uint64_t totalPseudoWhite = 0;

    // Aggregate missing explanation buckets
    std::unordered_map<std::string, MissInfo> missByBlackMove;

    for (auto bm : blackPseudo) {
        std::string bStr = decode(bm);
        Board::MoveState bst; Board posAfterBlack = root;
        posAfterBlack.makeMove(bStr, bst);
        // Discard illegal black (king left in check)
        if (gen.isKingInCheck(posAfterBlack, false)) continue; // black king in check => illegal

        auto whitePseudo = gen.generateAllMoves(posAfterBlack, true);
        std::vector<std::string> whiteLegal;
        std::vector<std::string> whiteIllegal;

        for (auto wm : whitePseudo) {
            std::string wStr = decode(wm);
            Board::MoveState wst; Board afterWhite = posAfterBlack;
            afterWhite.makeMove(wStr, wst);
            bool legal = !gen.isKingInCheck(afterWhite, true); // white just moved
            if (legal) whiteLegal.push_back(wStr); else whiteIllegal.push_back(wStr);
        }

        totalPseudoWhite += whitePseudo.size();
        totalLegalWhite += whiteLegal.size();

        if (!whiteIllegal.empty()) {
            auto &info = missByBlackMove[bStr];
            info.count += (int)whiteIllegal.size();
            // categorize by piece origin square
            for (auto &m : whiteIllegal) {
                int fromSq = ( (encodeMove(m,true) >> 6) & 0x3f );
                std::string piece = whitePieceAt(posAfterBlack, fromSq);
                info.pieceMap[piece]++;
            }
        }

        std::cout << bStr << " | pseudoW=" << whitePseudo.size() << " legalW=" << whiteLegal.size()
                  << " filtered=" << whitePseudo.size() - whiteLegal.size() << "\n";
    }

    std::cout << "\nTOTAL pseudo white replies: " << totalPseudoWhite << "\n";
    std::cout << "TOTAL legal white replies:  " << totalLegalWhite << "\n";
    std::cout << "Filtered (pseudo - legal):  " << (totalPseudoWhite - totalLegalWhite) << "\n";

    std::cout << "\nBLACK MOVES WITH MOST FILTERED WHITE REPLIES (top 10):\n";
    std::vector<std::pair<std::string,int>> ranking;
    ranking.reserve(missByBlackMove.size());
    for (auto &kv : missByBlackMove) ranking.push_back({kv.first, kv.second.count});
    std::sort(ranking.begin(), ranking.end(), [](auto &a, auto &b){return a.second > b.second;});
    for (size_t i=0;i<ranking.size() && i<10;i++) {
        auto &bm = ranking[i].first;
        auto &info = missByBlackMove[bm];
        std::cout << bm << " filtered=" << info.count << " pieces:";
        // piece breakdown
        for (auto &p : info.pieceMap) std::cout << " " << p.first << ":" << p.second;
        std::cout << "\n";
    }

    std::cout << "\nDONE.\n";
    return 0;
}
