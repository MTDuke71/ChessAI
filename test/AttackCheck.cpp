#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include <iostream>

bool isSquareAttacked(const Board& board, int square, bool byWhite) {
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(board, byWhite);
    
    for (auto move : moves) {
        int to = move & 0x3f;
        if (to == square) {
            return true;
        }
    }
    return false;
}

int main() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== CHECKING IF e1-d1 IS REALLY LEGAL ===\n";
    
    // Check if black pieces can attack d1
    bool d1Attacked = isSquareAttacked(board, 3, false); // d1 = square 3, attacked by black
    std::cout << "Can black attack d1? " << (d1Attacked ? "YES" : "NO") << "\n";
    
    if (d1Attacked) {
        std::cout << "e1-d1 should be ILLEGAL (king would be in check)\n";
    } else {
        std::cout << "e1-d1 appears to be legal\n";
    }
    
    // Let's also check the current position - is the king already in check?
    bool e1Attacked = isSquareAttacked(board, 4, false); // e1 = square 4, attacked by black
    std::cout << "Is white king currently in check on e1? " << (e1Attacked ? "YES" : "NO") << "\n";
    
    // Check some other squares for comparison
    std::cout << "Can black attack f1? " << (isSquareAttacked(board, 5, false) ? "YES" : "NO") << "\n";
    std::cout << "Can black attack c1? " << (isSquareAttacked(board, 2, false) ? "YES" : "NO") << "\n";
    std::cout << "Can black attack g1? " << (isSquareAttacked(board, 6, false) ? "YES" : "NO") << "\n";
    
    return 0;
}
