#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>
#include <map>

int main() {
    Board board;
    board.loadFEN("8/3k4/8/8/8/8/4K3/7R w - - 2 2");
    
    std::cout << "=== DEPTH 4 DELTA ANALYSIS ===" << std::endl;
    std::cout << "Position: 8/3k4/8/8/8/8/4K3/7R w - - 2 2" << std::endl;
    std::cout << std::endl;
    
    // Expected results from user's data
    std::map<std::string, int> expected = {
        {"e2d1", 752}, {"e2d2", 1110}, {"e2d3", 1110}, {"e2e1", 704}, {"e2e3", 1110},
        {"e2f1", 656}, {"e2f2", 1110}, {"e2f3", 1110}, {"h1a1", 1110}, {"h1b1", 1110},
        {"h1c1", 694}, {"h1d1", 790}, {"h1e1", 608}, {"h1f1", 1062}, {"h1g1", 1110},
        {"h1h2", 856}, {"h1h3", 1110}, {"h1h4", 1110}, {"h1h5", 1110}, {"h1h6", 694},
        {"h1h7", 832}, {"h1h8", 694}
    };
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, true);
    
    int totalActual = 0;
    int totalExpected = 0;
    int totalDelta = 0;
    
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
        int actual = perft(copy, gen, 3);
        copy.unmakeMove(state);
        
        int exp = expected[key];
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
    std::cout << "- Percentage difference: " << 
                 (100.0 * totalDelta / totalExpected) << "%" << std::endl;
    
    return 0;
}
