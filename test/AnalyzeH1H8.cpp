#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>
#include <map>

int main() {
    Board board;
    board.loadFEN("4k2R/8/8/8/8/8/8/4K3 b - - 1 1");
    
    std::cout << "=== ANALYSIS: h1-h8 Position ===" << std::endl;
    std::cout << "Position: 4k2R/8/8/8/8/8/8/4K3 b - - 1 1" << std::endl;
    std::cout << "This is the position after h1-h8 from 4k3/8/8/8/8/8/8/4K2R w K - 0 1" << std::endl;
    std::cout << std::endl;
    
    board.printBoard();
    
    // Expected results from user's data (Depth 5)
    std::map<std::string, int> expected = {
        {"e8d7", 12340}, {"e8e7", 12331}, {"e8f7", 11657}
    };
    int expectedTotal = 36328;
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, false); // Black to move
    
    int totalActual = 0;
    int totalExpected = 0;
    int totalDelta = 0;
    
    std::cout << "=== DEPTH 5 ANALYSIS ===" << std::endl;
    std::cout << "Black to move (" << moves.size() << " legal moves)" << std::endl;
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
        int actual = perft(copy, gen, 4);  // Depth 4 from this position
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
    std::cout << "=== SUMMARY ===" << std::endl;
    std::cout << "- Expected total: " << totalExpected << std::endl;
    std::cout << "- Actual total: " << totalActual << std::endl;
    std::cout << "- Total delta: " << totalDelta << std::endl;
    std::cout << "- This should match the h1-h8 discrepancy of -30 nodes" << std::endl;
    
    // Verify we have exactly 3 moves for the black king
    if (moves.size() != 3) {
        std::cout << "⚠️  Warning: Expected 3 moves for black king, found " << moves.size() << std::endl;
        std::cout << "All moves found:" << std::endl;
        for (const auto& move : moves) {
            std::cout << "  " << decodeMove(move) << std::endl;
        }
    }
    
    return 0;
}
