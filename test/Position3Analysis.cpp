#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>
#include <map>

int main() {
    Board board;
    board.loadFEN("4k3/8/8/8/8/8/8/4K2R w K - 0 1");
    
    std::cout << "=== POSITION 3 COMPREHENSIVE ANALYSIS ===" << std::endl;
    std::cout << "Position: 4k3/8/8/8/8/8/8/4K2R w K - 0 1" << std::endl;
    std::cout << "Classic perft position with castling rights" << std::endl;
    std::cout << std::endl;
    
    board.printBoard();
    
    // Expected results from user's data (Depth 6)
    std::map<std::string, int> expected = {
        {"e1d1", 47643}, {"e1d2", 81423}, {"e1e2", 81484}, {"e1f1", 42182}, 
        {"e1f2", 79592}, {"e1g1", 26553}, {"h1f1", 22226}, {"h1g1", 39089}, 
        {"h1h2", 60485}, {"h1h3", 63965}, {"h1h4", 63965}, {"h1h5", 61436}, 
        {"h1h6", 48719}, {"h1h7", 9553}, {"h1h8", 36328}
    };
    int expectedTotal = 764643;
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, true);
    
    int totalActual = 0;
    int totalExpected = 0;
    int totalDelta = 0;
    
    std::cout << "=== DEPTH 6 ANALYSIS ===" << std::endl;
    std::cout << "White to move (" << moves.size() << " legal moves)" << std::endl;
    std::cout << std::endl;
    std::cout << "Move     | Expected | Actual | Delta  | Ratio%" << std::endl;
    std::cout << "---------|----------|--------|--------|--------" << std::endl;
    
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
        int actual = perft(copy, gen, 5);  // Depth 5 from this position
        copy.unmakeMove(state);
        
        int exp = expected.count(key) ? expected[key] : 0;
        int delta = actual - exp;
        double ratio = exp > 0 ? (100.0 * actual / exp) : 0.0;
        
        totalActual += actual;
        totalExpected += exp;
        totalDelta += delta;
        
        printf("%-8s | %8d | %6d | %6d | %6.2f%%\n", 
               moveStr.c_str(), exp, actual, delta, ratio);
    }
    
    std::cout << "---------|----------|--------|--------|--------" << std::endl;
    printf("TOTAL    | %8d | %6d | %6d | %6.2f%%\n", 
           totalExpected, totalActual, totalDelta, 
           100.0 * totalActual / totalExpected);
    
    std::cout << std::endl;
    std::cout << "=== SUMMARY ===" << std::endl;
    std::cout << "- Expected total: " << totalExpected << std::endl;
    std::cout << "- Actual total: " << totalActual << std::endl;
    std::cout << "- Total delta: " << totalDelta << std::endl;
    std::cout << "- Percentage difference: " << 
                 (100.0 * totalDelta / totalExpected) << "%" << std::endl;
    std::cout << "- Original discrepancy: 764643 - 764613 = 30 nodes" << std::endl;
    std::cout << "- Current discrepancy: " << totalExpected << " - " << totalActual 
              << " = " << (totalExpected - totalActual) << " nodes" << std::endl;
    
    // Check if we match the original calculation
    if (totalActual == 764613) {
        std::cout << "✅ Perfect match with original calculation (764613)" << std::endl;
    } else {
        std::cout << "❌ Difference from original: " << (totalActual - 764613) << std::endl;
    }
    
    return 0;
}
