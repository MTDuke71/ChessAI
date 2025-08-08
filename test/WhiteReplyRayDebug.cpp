#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Magic.h"
#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>

static inline int lsbIndex(uint64_t bb){ return bb ? __builtin_ctzll(bb) : -1; }

static std::string sqName(int sq){
    std::string s; s+= char('a'+(sq%8)); s+= char('1'+(sq/8)); return s;
}

static std::string decode(uint16_t m){
    int to = m & 0x3f;
    int from = (m>>6)&0x3f;
    int promo = (m>>12)&0x3;
    int special = (m>>14)&0x3;
    if(special==3){ // castling
        if(to-from==2) return "O-O"; if(from-to==2) return "O-O-O";
    }
    std::string s = sqName(from) + std::string("-") + sqName(to);
    if(special==1){ static const char p[4] = {'n','b','r','q'}; s+=p[promo]; }
    return s;
}

static void collectMoveSet(const std::vector<uint16_t>& moves, std::unordered_set<std::string>& out){
    for(auto m: moves) out.insert(decode(m));
}

static void listSliderMissing(const Board& pos, const std::unordered_set<std::string>& moveSet){
    uint64_t occ = pos.getWhitePieces() | pos.getBlackPieces();
    auto checkPiece = [&](uint64_t bb, bool bishopLike, bool rookLike){
        while(bb){
            int from = lsbIndex(bb); bb &= bb-1;
            uint64_t attacks = 0ULL;
            if(bishopLike) attacks |= Magic::getBishopAttacks(from, occ);
            if(rookLike)   attacks |= Magic::getRookAttacks(from, occ);
            // eliminate own pieces
            attacks &= ~pos.getWhitePieces();
            // iterate targets
            uint64_t a = attacks;
            while(a){
                int to = lsbIndex(a); a &= a-1;
                std::string mv = sqName(from) + std::string("-") + sqName(to);
                if(!moveSet.count(mv)){
                    std::cout << "  MISSING slider move: " << mv;
                    bool capture = ( (1ULL<<to) & pos.getBlackPieces());
                    if(capture) std::cout << " (capture)";
                    std::cout << "\n";
                }
            }
        }
    };
    std::cout << " Checking bishops...\n";
    checkPiece(pos.getWhiteBishops(), true, false);
    std::cout << " Checking rooks...\n";
    checkPiece(pos.getWhiteRooks(), false, true);
    std::cout << " Checking queens...\n";
    checkPiece(pos.getWhiteQueens(), true, true);
}

int main(){
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board; board.loadFEN(fen);
    // Apply e2-d1 (bishop) move
    Board::MoveState st; board.makeMove("e2-d1", st);

    MoveGenerator gen;
    auto blackMoves = gen.generateAllMoves(board, false);
    std::cout << "Black pseudo moves after e2-d1: " << blackMoves.size() << "\n";

    // Pick a representative black move with high expected branching: choose a6-c8 if present
    std::string targetBlack = "a6-c8"; // one of the 42-response moves observed
    bool found=false;
    for(auto m: blackMoves){ auto s = decode(m); if(s==targetBlack){ found=true; break; } }
    if(!found && !blackMoves.empty()) targetBlack = decode(blackMoves[0]);
    std::cout << "Using black reply: " << targetBlack << "\n";

    Board::MoveState stB; board.makeMove(targetBlack, stB);

    // Generate white replies
    auto whitePseudo = gen.generateAllMoves(board, true);
    std::unordered_set<std::string> whiteSet; whiteSet.reserve(whitePseudo.size()*2);
    collectMoveSet(whitePseudo, whiteSet);
    std::cout << "White pseudo replies count: " << whitePseudo.size() << "\n";

    // List slider missing moves
    std::cout << "\n=== Slider coverage check ===\n";
    listSliderMissing(board, whiteSet);

    std::cout << "\nDone.\n";
    return 0;
}
