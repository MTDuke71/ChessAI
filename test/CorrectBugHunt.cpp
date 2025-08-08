#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>
#include <map>

int main() {
    // Correct position after h1-h8, e8-e7
    Board board;
    board.loadFEN("7R/4k3/8/8/8/8/8/4K3 w - - 2 2");
    
    std::cout << "=== CORRECTED BUG ANALYSIS: After h1-h8, e8-e7 ===" << std::endl;
    std::cout << "Position: 7R/4k3/8/8/8/8/8/4K3 w - - 2 2" << std::endl;
    std::cout << "This position shows a 14-node discrepancy (Expected: 12331)" << std::endl;
    std::cout << std::endl;
    
    board.printBoard();
    
    // Expected results from user's data (Depth 4)
    std::map<std::string, int> expected = {
        {"e1d1", 626}, {"e1d2", 728}, {"e1e2", 728}, {"e1f1", 626}, {"e1f2", 728},
        {"h8a8", 626}, {"h8b8", 626}, {"h8c8", 578}, {"h8d8", 379}, {"h8e8", 399},
        {"h8f8", 379}, {"h8g8", 578}, {"h8h1", 697}, {"h8h2", 935}, {"h8h3", 935},
        {"h8h4", 935}, {"h8h5", 854}, {"h8h6", 473}, {"h8h7", 501}
    };
    int expectedTotal = 12331;
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, true); // White to move
    
    int totalActual = 0;
    int totalExpected = 0;
    int totalDelta = 0;
    
    std::cout << "=== DEPTH 4 BUG HUNT ===" << std::endl;
    std::cout << "White to move (" << moves.size() << " legal moves)" << std::endl;
    std::cout << std::endl;
    std::cout << "Move     | Expected | Actual | Delta | Status" << std::endl;
    std::cout << "---------|----------|--------|-------|--------" << std::endl;
    
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
        int actual = perft(copy, gen, 3);  // Depth 3 from this position
        copy.unmakeMove(state);
        
        int exp = expected.count(key) ? expected[key] : 0;
        int delta = actual - exp;
        std::string status = (delta == 0) ? "✅ OK" : (delta < 0 ? "❌ LOW" : "⚠️ HIGH");
        
        totalActual += actual;
        totalExpected += exp;
        totalDelta += delta;
        
        printf("%-8s | %8d | %6d | %5d | %s\n", 
               moveStr.c_str(), exp, actual, delta, status.c_str());
    }
    
    std::cout << "---------|----------|--------|-------|--------" << std::endl;
    printf("TOTAL    | %8d | %6d | %5d | %s\n", 
           totalExpected, totalActual, totalDelta,
           (totalDelta == 0) ? "✅ PERFECT" : "❌ BUG FOUND");
    
    std::cout << std::endl;
    std::cout << "=== BUG ANALYSIS ===" << std::endl;
    std::cout << "- Expected total: " << totalExpected << std::endl;
    std::cout << "- Actual total: " << totalActual << std::endl;
    std::cout << "- Total delta: " << totalDelta << std::endl;
    std::cout << "- This should match the e8-e7 discrepancy of -14 nodes" << std::endl;
    
    if (totalDelta != 0) {
        std::cout << std::endl;
        std::cout << "🐛 BUG DETAILS:" << std::endl;
        std::cout << "Missing " << (-totalDelta) << " nodes in this position." << std::endl;
        
        // List all problematic moves
        std::cout << std::endl;
        std::cout << "Moves with discrepancies:" << std::endl;
        for (const auto& move : moves) {
            std::string moveStr = decodeMove(move);
            std::string key = moveStr;
            if (key.find('-') != std::string::npos) {
                key.erase(key.find('-'), 1);
            }
            
            Board copy = board;
            Board::MoveState state;
            copy.makeMove(move, state);
            int actual = perft(copy, gen, 3);
            copy.unmakeMove(state);
            
            int exp = expected.count(key) ? expected[key] : 0;
            int delta = actual - exp;
            
            if (delta != 0) {
                std::cout << "- " << moveStr << ": Expected " << exp << ", Got " << actual 
                         << " (delta: " << delta << ")" << std::endl;
            }
        }
    }
    
    return 0;
}
