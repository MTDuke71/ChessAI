#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <map>
#include <iomanip>

int main() {
    std::cout << "=== A2-A3 DEPTH 2 ANALYSIS ===\n";
    
    // Start with initial position and make a2-a3
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    Board::MoveState state;
    board.makeMove("a2-a3", state);
    
    std::cout << "After a2-a3, FEN: ";
    std::cout << "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q1p/1PPBBPPP/R3K2R b KQkq - 0 1\n\n";
    
    // Expected values from user's data
    std::map<std::string, int> expected = {
        {"a6-b5", 49}, {"a6-b7", 50}, {"a6-c4", 48}, {"a6-c8", 50}, {"a6-d3", 48}, {"a6-e2", 43},
        {"a8-b8", 50}, {"a8-c8", 50}, {"a8-d8", 50}, {"b4-a3", 51}, {"b4-b3", 49}, {"b4-c3", 48},
        {"b6-a4", 49}, {"b6-c4", 48}, {"b6-c8", 50}, {"b6-d5", 50}, {"c7-c5", 51}, {"c7-c6", 51},
        {"d7-d6", 49}, {"e6-d5", 50}, {"e7-c5", 50}, {"e7-d6", 49}, {"e7-d8", 50}, {"e7-f8", 50},
        {"e8-c8", 50}, {"e8-d8", 50}, {"e8-f8", 50}, {"e8-g8", 50}, {"f6-d5", 51}, {"f6-e4", 53},
        {"f6-g4", 49}, {"f6-g8", 51}, {"f6-h5", 51}, {"f6-h7", 51}, {"g6-g5", 49}, {"g7-f8", 50},
        {"g7-h6", 50}, {"h3-g2", 48}, {"h8-f8", 50}, {"h8-g8", 50}, {"h8-h4", 50}, {"h8-h5", 50},
        {"h8-h6", 50}, {"h8-h7", 50}
    };
    
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(board, false); // Black to move
    
    std::cout << "Move Analysis (Actual vs Expected):\n";
    std::cout << "Move      | Actual | Expected | Diff\n";
    std::cout << "----------|--------|----------|-----\n";
    
    int totalActual = 0;
    int totalExpected = 0;
    int differences = 0;
    
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        if (!board.isMoveLegal(moveStr)) continue;
        
        Board copy = board;
        Board::MoveState moveState;
        copy.makeMove(moveStr, moveState);
        
        uint64_t actual = perft(copy, gen, 1); // Depth 1 from this position
        totalActual += actual;
        
        // Convert castling notation for lookup
        std::string lookupMove = moveStr;
        if (moveStr == "O-O") lookupMove = "e8-g8";
        else if (moveStr == "O-O-O") lookupMove = "e8-c8";
        
        if (expected.find(lookupMove) != expected.end()) {
            int exp = expected[lookupMove];
            int diff = (int)actual - exp;
            totalExpected += exp;
            
            std::cout << std::left << std::setw(10) << moveStr 
                     << "| " << std::setw(6) << actual 
                     << " | " << std::setw(8) << exp 
                     << " | " << std::setw(4) << diff << "\n";
            
            if (diff != 0) {
                differences++;
            }
        } else {
            std::cout << std::left << std::setw(10) << moveStr 
                     << "| " << std::setw(6) << actual 
                     << " | MISSING  | ?\n";
            differences++;
        }
    }
    
    std::cout << "\n=== SUMMARY ===\n";
    std::cout << "Total Actual:   " << totalActual << "\n";
    std::cout << "Total Expected: " << totalExpected << "\n";
    std::cout << "Total Diff:     " << (totalActual - totalExpected) << "\n";
    std::cout << "Moves with differences: " << differences << "\n";
    std::cout << "Expected move count: " << expected.size() << "\n";
    
    // Check for any moves we generate that aren't in expected
    std::cout << "\n=== MOVES NOT IN EXPECTED LIST ===\n";
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        if (!board.isMoveLegal(moveStr)) continue;
        
        std::string lookupMove = moveStr;
        if (moveStr == "O-O") lookupMove = "e8-g8";
        else if (moveStr == "O-O-O") lookupMove = "e8-c8";
        
        if (expected.find(lookupMove) == expected.end()) {
            std::cout << "Extra move: " << moveStr << "\n";
        }
    }
    
    return 0;
}
