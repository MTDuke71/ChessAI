#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>
#include <map>

int main() {
    Board board;
    board.loadFEN("8/3k3R/8/8/8/8/4K3/8 b - - 3 2");
    
    std::cout << "=== ANALYSIS: h1-h7 Position ===" << std::endl;
    std::cout << "Position: 8/3k3R/8/8/8/8/4K3/8 b - - 3 2" << std::endl;
    std::cout << "This is the position after h1-h7 from 8/3k4/8/8/8/8/4K3/7R w - - 2 2" << std::endl;
    std::cout << std::endl;
    
    board.printBoard();
    
    // Expected results from user's data
    std::map<std::string, int> expected = {
        {"d7c6", 124}, {"d7c8", 64}, {"d7d6", 124}, 
        {"d7d8", 64}, {"d7e6", 124}, {"d7e8", 64}
    };
    int expectedTotal = 564;  // This matches our actual total
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, false); // Black to move
    
    int totalActual = 0;
    int totalExpected = 0;
    int totalDelta = 0;
    
    std::cout << "=== DEPTH 3 ANALYSIS ===" << std::endl;
    std::cout << "Black to move (6 legal moves expected)" << std::endl;
    std::cout << std::endl;
    std::cout << "Move     | Expected | Actual | Delta" << std::endl;
    std::cout << "---------|----------|--------|-------" << std::endl;
    
    for (const auto& move : moves) {
        std::string moveStr = decodeMove(move);
        
        // Convert to format used in expected data
        std::string key = moveStr;
        if (key.find('-') != std::string::npos) {
            key.erase(key.find('-'), 1);
        }
        
        Board copy = board;
        Board::MoveState state;
        copy.makeMove(move, state);
        int actual = perft(copy, gen, 2);  // Depth 2 from this position
        copy.unmakeMove(state);
        
        int exp = expected.count(key) ? expected[key] : 0;
        int delta = actual - exp;
        
        totalActual += actual;
        totalExpected += exp;
        totalDelta += delta;
        
        printf("%-8s | %8d | %6d | %6d\n", 
               moveStr.c_str(), exp, actual, delta);
    }
    
    std::cout << "---------|----------|--------|-------" << std::endl;
    printf("TOTAL    | %8d | %6d | %6d\n", 
           totalExpected, totalActual, totalDelta);
    
    std::cout << std::endl;
    std::cout << "Summary:" << std::endl;
    std::cout << "- Total expected: " << totalExpected << std::endl;
    std::cout << "- Total actual: " << totalActual << std::endl;
    std::cout << "- Total delta: " << totalDelta << std::endl;
    std::cout << "- Our implementation gives: " << totalActual << " nodes" << std::endl;
    std::cout << "- User reported actual: 564 nodes" << std::endl;
    std::cout << "- Expected from reference: 832 nodes" << std::endl;
    std::cout << "- Delta from reference: " << (totalActual - 832) << std::endl;
    
    return 0;
}
