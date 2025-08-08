#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>

int main() {
    std::cout << "=== ANALYZING E5-F7 MOVE DISCREPANCY ===\n";
    
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    std::cout << "Starting position loaded\n";
    std::cout << "Looking for e5-f7 move...\n";
    
    // Make the e5-f7 move
    Board copy = board;
    Board::MoveState state;
    
    std::cout << "Making move e5-f7...\n";
    copy.makeMove("e5-f7", state);
    
    std::cout << "Computing depth 2 perft from e5-f7 position...\n";
    MoveGenerator gen;
    uint64_t actual = perft(copy, gen, 2);
    
    std::cout << "Actual result: " << actual << "\n";
    std::cout << "Expected result: 2080\n";
    std::cout << "Difference: " << (int64_t)(actual - 2080) << "\n";
    
    if (actual != 2080) {
        std::cout << "\nAnalyzing black's moves after e5-f7...\n";
        auto moves = gen.generateAllMoves(copy, false); // Black to move
        
        int legalMoves = 0;
        for (auto move : moves) {
            std::string moveStr = decodeMove(move);
            if (copy.isMoveLegal(moveStr)) {
                legalMoves++;
            }
        }
        
        std::cout << "Black has " << legalMoves << " legal moves\n";
        
        // Let's also check what piece is on f7 after the move
        std::cout << "\nBoard state after e5-f7:\n";
        std::cout << "White knights: " << std::hex << copy.getWhiteKnights() << std::dec << "\n";
        std::cout << "Black pawns: " << std::hex << copy.getBlackPawns() << std::dec << "\n";
        
        // Check if f7 has the white knight
        uint64_t f7 = 1ULL << 53; // f7 = square 53
        if (copy.getWhiteKnights() & f7) {
            std::cout << "✓ White knight correctly placed on f7\n";
        } else {
            std::cout << "✗ White knight NOT on f7!\n";
        }
    }
    
    return 0;
}
