#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <map>
#include <iomanip>

int main() {
    std::cout << "=== ANALYZING E5-F7 DEPTH 2 BREAKDOWN ===\n";
    
    // Start with the test position
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    // Make the e5-f7 move
    Board::MoveState state;
    board.makeMove("e5-f7", state);
    
    std::cout << "After e5-f7, analyzing black's responses...\n\n";
    
    // Expected values from user's data
    std::map<std::string, int> expected = {
        {"a6-b5", 47}, {"a6-b7", 48}, {"a6-c4", 46}, {"a6-c8", 48}, {"a6-d3", 46}, {"a6-e2", 41},
        {"a8-b8", 48}, {"a8-c8", 48}, {"a8-d8", 48}, {"b4-b3", 49}, {"b4-c3", 48}, {"b6-a4", 47},
        {"b6-c4", 46}, {"b6-c8", 48}, {"b6-d5", 48}, {"c7-c5", 49}, {"c7-c6", 49}, {"d7-d6", 47},
        {"e6-d5", 48}, {"e6-e5", 46}, {"e7-c5", 48}, {"e7-d6", 47}, {"e7-d8", 48}, {"e7-f7", 42},
        {"e7-f8", 48}, {"e8-f7", 42}, {"e8-f8", 48}, {"e8-g8", 48}, {"f6-d5", 48}, {"f6-e4", 49},
        {"f6-g4", 46}, {"f6-g8", 48}, {"f6-h5", 48}, {"f6-h7", 48}, {"g6-g5", 47}, {"g7-f8", 48},
        {"g7-h6", 48}, {"h3-g2", 46}, {"h8-f8", 48}, {"h8-g8", 48}, {"h8-h4", 48}, {"h8-h5", 48},
        {"h8-h6", 48}, {"h8-h7", 48}
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
    std::cout << "Expected Total: 2080\n";
    std::cout << "Moves with differences: " << differences << "\n";
    
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
