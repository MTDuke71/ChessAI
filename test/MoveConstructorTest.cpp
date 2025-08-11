#include "../src/Board.h"
#include "../src/FastMoveGenerator.h"
#include <iostream>

int main() {
    std::cout << "=== MOVE CONSTRUCTOR TEST ===\n";
    
    // Test Move constructor directly
    std::cout << "Testing Move constructor:\n";
    
    FastMoveGenerator::Move testMove1(14, 6, 0, 4, false); // g2-g1, Queen promotion
    FastMoveGenerator::Move testMove2(14, 6, 0, 3, false); // g2-g1, Rook promotion
    FastMoveGenerator::Move testMove3(14, 6, 0, 2, false); // g2-g1, Bishop promotion
    FastMoveGenerator::Move testMove4(14, 6, 0, 1, false); // g2-g1, Knight promotion
    
    std::cout << "Move 1: " << testMove1.toAlgebraic() << " (promotion=" << testMove1.promotion() << ")\n";
    std::cout << "Move 2: " << testMove2.toAlgebraic() << " (promotion=" << testMove2.promotion() << ")\n";
    std::cout << "Move 3: " << testMove3.toAlgebraic() << " (promotion=" << testMove3.promotion() << ")\n";
    std::cout << "Move 4: " << testMove4.toAlgebraic() << " (promotion=" << testMove4.promotion() << ")\n";
    
    std::cout << "\nNow testing with actual board:\n";
    
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q2/PPPBBPpP/R3K2R b Qkq - 1 2");
    
    // Generate moves directly with FastMoveGenerator
    FastMoveGenerator fastGen;
    FastMoveGenerator::MoveList moveList;
    fastGen.generateLegalMoves(board, false, moveList); // Black to move
    
    std::cout << "Moves from g2 (square 14):\n";
    for (int i = 0; i < moveList.count; i++) {
        const auto& move = moveList.moves[i];
        if (move.from() == 14) { // g2 square
            std::cout << "  Move " << i << ": " << move.toAlgebraic() 
                      << " (from=" << move.from() << " to=" << move.to() 
                      << " promotion=" << move.promotion() << ")\n";
        }
    }
    
    return 0;
}
