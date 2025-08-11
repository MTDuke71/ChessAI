#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::cout << "=== PAWN PROMOTION TEST ===\n";
    
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q2/PPPBBPpP/R3K2R b Qkq - 1 2");
    
    std::cout << "Position: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q2/PPPBBPpP/R3K2R b Qkq - 1 2\n";
    std::cout << "Black to move - pawn on g2 should promote\n\n";
    
    // Generate moves with MoveGenerator (which uses FastMoveGenerator internally)
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(board, false); // Black to move
    
    std::cout << "Generated moves (via convertMoves):\n";
    int promotionCount = 0;
    
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        
        // Check if this is a g2 move
        if (moveStr.find("g2") == 0) {
            std::cout << "  " << moveStr;
            if (moveStr.length() > 5) { // Has promotion piece
                std::cout << " (PROMOTION)";
                promotionCount++;
            } else {
                std::cout << " (SIMPLE MOVE - ERROR!)";
            }
            std::cout << "\n";
        }
    }
    
    std::cout << "\nTotal g2 promotion moves found: " << promotionCount << "\n";
    std::cout << "Expected: 8 (g2g1q,r,b,n + g2h1q,r,b,n)\n";
    
    if (promotionCount == 0) {
        std::cout << "✗ Missing promotion moves\n";
    } else if (promotionCount == 8) {
        std::cout << "✓ All promotion moves found\n";
    } else {
        std::cout << "⚠ Partial promotion moves found\n";
    }
    
    return 0;
}
