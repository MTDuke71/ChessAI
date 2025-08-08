#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <iomanip>

int main() {
    std::cout << "=== DETAILED CASTLING DEPTH 2 ANALYSIS ===\n";
    
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    // Expected values from user's data
    std::map<std::string, int> expected = {
        {"a6-b5", 47}, {"a6-b7", 48}, {"a6-c4", 46}, {"a6-c8", 48}, {"a6-d3", 46}, {"a6-e2", 45},
        {"a8-b8", 48}, {"a8-c8", 48}, {"a8-d8", 48}, {"b4-b3", 49}, {"b4-c3", 48},
        {"b6-a4", 47}, {"b6-c4", 46}, {"b6-c8", 48}, {"b6-d5", 48}, {"c7-c5", 49}, {"c7-c6", 49},
        {"d7-d6", 47}, {"e6-d5", 48}, {"e7-c5", 48}, {"e7-d6", 47}, {"e7-d8", 48}, {"e7-f8", 48},
        {"e8-c8", 48}, {"e8-d8", 48}, {"e8-f8", 48}, {"e8-g8", 48}, {"f6-d5", 49}, {"f6-e4", 51},
        {"f6-g4", 47}, {"f6-g8", 49}, {"f6-h5", 49}, {"f6-h7", 49}, {"g6-g5", 47},
        {"g7-f8", 48}, {"g7-h6", 48}, {"h3-g2", 48}, {"h8-f8", 48}, {"h8-g8", 48},
        {"h8-h4", 48}, {"h8-h5", 48}, {"h8-h6", 48}, {"h8-h7", 48}
    };
    
    // Make the kingside castling move
    Board::MoveState state;
    board.makeMove("O-O", state);
    
    std::cout << "Position after O-O: " << board.getFEN() << "\n\n";
    
    MoveGenerator gen;
    auto blackMoves = gen.generateAllMoves(board, false);
    
    uint64_t totalActual = 0;
    uint64_t totalExpected = 0;
    int discrepancies = 0;
    
    std::cout << "Black Move   | Actual | Expected | Diff\n";
    std::cout << "-------------|--------|----------|-----\n";
    
    for (auto move : blackMoves) {
        std::string moveStr = decodeMove(move);
        if (!board.isMoveLegal(moveStr)) continue;
        
        Board copy = board;
        Board::MoveState state2;
        copy.makeMove(moveStr, state2);
        
        uint64_t actual = perft(copy, gen, 1);
        totalActual += actual;
        
        // Convert castling notation for lookup
        std::string lookupMove = moveStr;
        if (moveStr == "O-O") lookupMove = "e8-g8";
        else if (moveStr == "O-O-O") lookupMove = "e8-c8";
        
        if (expected.find(lookupMove) != expected.end()) {
            int exp = expected[lookupMove];
            int diff = (int)actual - exp;
            totalExpected += exp;
            
            std::cout << std::left << std::setw(12) << moveStr 
                     << " | " << std::setw(6) << actual 
                     << " | " << std::setw(8) << exp 
                     << " | " << std::setw(4) << diff;
            
            if (diff != 0) {
                std::cout << " ⚠️";
                discrepancies++;
            }
            std::cout << "\n";
        } else {
            std::cout << std::left << std::setw(12) << moveStr 
                     << " | " << std::setw(6) << actual 
                     << " | UNKNOWN  | ?\n";
        }
    }
    
    std::cout << "\n=== SUMMARY ===\n";
    std::cout << "Total Actual:   " << totalActual << "\n";
    std::cout << "Total Expected: " << totalExpected << "\n";
    std::cout << "Total Diff:     " << (int64_t)totalActual - (int64_t)totalExpected << "\n";
    std::cout << "Discrepancies:  " << discrepancies << " out of " << expected.size() << " moves\n";
    
    return 0;
}
