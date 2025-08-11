#include "../src/Board.h"
#include "../src/FastMoveGenerator.h"
#include <iostream>

int main() {
    std::cout << "=== RAW FAST MOVE GENERATOR TEST ===\n";
    
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q2/PPPBBPpP/R3K2R b Qkq - 1 2");
    
    std::cout << "Position: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q2/PPPBBPpP/R3K2R b Qkq - 1 2\n";
    std::cout << "Black to move - pawn on g2 should promote\n\n";
    
    // Generate moves directly with FastMoveGenerator
    FastMoveGenerator fastGen;
    FastMoveGenerator::MoveList moveList;
    fastGen.generateLegalMoves(board, false, moveList); // Black to move
    
    std::cout << "Raw FastMoveGenerator moves for g2:\n";
    int promotionCount = 0;
    
    for (int i = 0; i < moveList.count; i++) {
        const auto& move = moveList.moves[i];
        if (move.from() == 14) { // g2 square (rank 1, file 6: 1*8+6 = 14)
            std::string algebraic = move.toAlgebraic();
            std::cout << "  Move " << i << ": " << algebraic;
            std::cout << " (from=" << move.from() << " to=" << move.to() 
                      << " promotion=" << move.promotion() << ")";
            if (move.promotion() > 0) {
                std::cout << " ✓ PROMOTION";
                promotionCount++;
            } else {
                std::cout << " ✗ SIMPLE MOVE";
            }
            std::cout << "\n";
        }
    }
    
    std::cout << "\nTotal g2 promotion moves from FastMoveGenerator: " << promotionCount << "\n";
    std::cout << "Expected: 8 (g2g1q,r,b,n + g2h1q,r,b,n)\n";
    
    if (promotionCount == 0) {
        std::cout << "✗ FastMoveGenerator not generating promotion moves\n";
    } else if (promotionCount == 8) {
        std::cout << "✓ FastMoveGenerator generating correct promotion moves\n";
    } else {
        std::cout << "⚠ FastMoveGenerator generating partial promotion moves\n";
    }
    
    return 0;
}
