#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>
#include <map>

int main() {
    Board board;
    board.loadFEN("8/7R/2k5/8/8/8/4K3/8 w - - 4 3");
    
    std::cout << "=== ANALYSIS: Position after d7-c6 ===" << std::endl;
    std::cout << "Position: 8/7R/2k5/8/8/8/4K3/8 w - - 4 3" << std::endl;
    std::cout << "This comes from h1-h7, d7-c6 sequence" << std::endl;
    std::cout << std::endl;
    
    board.printBoard();
    
    // Expected results from user's data
    std::map<std::string, int> expected = {
        {"e2d1", 5}, {"e2d2", 5}, {"e2d3", 5}, {"e2e1", 5}, {"e2e3", 5},
        {"e2f1", 5}, {"e2f2", 5}, {"e2f3", 5}, {"h7a7", 5}, {"h7b7", 4},
        {"h7c7", 5}, {"h7d7", 4}, {"h7e7", 5}, {"h7f7", 5}, {"h7g7", 5},
        {"h7h1", 8}, {"h7h2", 8}, {"h7h3", 8}, {"h7h4", 8}, {"h7h5", 5},
        {"h7h6", 6}, {"h7h8", 8}
    };
    int expectedTotal = 124;
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, true); // White to move
    
    int totalActual = 0;
    int totalExpected = 0;
    int totalDelta = 0;
    
    std::cout << "=== DEPTH 2 ANALYSIS ===" << std::endl;
    std::cout << "White to move (" << moves.size() << " legal moves)" << std::endl;
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
        int actual = perft(copy, gen, 1);  // Depth 1 from this position
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
    std::cout << "- User reported actual: 124 nodes" << std::endl;
    
    // Let's also check what we get at depth 1
    std::cout << std::endl;
    std::cout << "=== DEPTH 1 VERIFICATION ===" << std::endl;
    std::cout << "Total moves from this position: " << moves.size() << std::endl;
    
    return 0;
}
