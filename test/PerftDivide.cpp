#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <map>

int main() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    
    Board board;
    board.loadFEN(fen);
    MoveGenerator gen;
    
    std::cout << "=== PERFT DIVIDE DEPTH 3 ===\n";
    std::cout << "FEN: " << fen << "\n\n";
    
    uint64_t total = perftDivide(board, gen, 3);
    
    std::cout << "\nTotal: " << total << "\n";
    std::cout << "Expected: 97862\n";
    std::cout << "Difference: " << (int64_t)total - 97862 << "\n";
    
    return 0;
}
