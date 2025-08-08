#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>
#include <map>

int main() {
    Board board;
    board.loadFEN("4R3/4k3/8/8/8/8/8/4K3 b - - 3 2");
    
    std::cout << "=== FINAL BUG ANALYSIS: h8-e8 Position ===" << std::endl;
    std::cout << "Position: 4R3/4k3/8/8/8/8/8/4K3 b - - 3 2" << std::endl;
    std::cout << "This position is missing 14 nodes (Expected: 399, Getting: 385)" << std::endl;
    std::cout << std::endl;
    
    board.printBoard();
    
    // Expected results from user's data (Depth 3)
    std::map<std::string, int> expected = {
        {"e7d6", 104}, {"e7d7", 83}, {"e7e8", 25}, {"e7f6", 104}, {"e7f7", 83}
    };
    int expectedTotal = 399;
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, false); // Black to move
    
    int totalActual = 0;
    int totalExpected = 0;
    int totalDelta = 0;
    
    std::cout << "=== DEPTH 3 BUG HUNT ===" << std::endl;
    std::cout << "Black to move (" << moves.size() << " legal moves)" << std::endl;
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
        int actual = perft(copy, gen, 2);  // Depth 2 from this position
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
    std::cout << "=== CRITICAL BUG ANALYSIS ===" << std::endl;
    std::cout << "- Expected total: " << totalExpected << std::endl;
    std::cout << "- Actual total: " << totalActual << std::endl;
    std::cout << "- Total delta: " << totalDelta << std::endl;
    std::cout << "- This MUST equal -14 to match h8-e8 discrepancy" << std::endl;
    
    if (totalDelta == -14) {
        std::cout << "✅ CONFIRMED: This is the source of the 14-node bug!" << std::endl;
    } else {
        std::cout << "❌ MISMATCH: Expected -14 delta, got " << totalDelta << std::endl;
    }
    
    if (totalDelta != 0) {
        std::cout << std::endl;
        std::cout << "🐛 SPECIFIC BUG LOCATIONS:" << std::endl;
        
        for (const auto& move : moves) {
            std::string moveStr = decodeMove(move);
            std::string key = moveStr;
            if (key.find('-') != std::string::npos) {
                key.erase(key.find('-'), 1);
            }
            
            Board copy = board;
            Board::MoveState state;
            copy.makeMove(move, state);
            int actual = perft(copy, gen, 2);
            copy.unmakeMove(state);
            
            int exp = expected.count(key) ? expected[key] : 0;
            int delta = actual - exp;
            
            if (delta != 0) {
                std::cout << "- " << moveStr << ": Expected " << exp << ", Got " << actual 
                         << " (" << delta << " nodes)" << std::endl;
                
                // Show the position after this move for debugging
                std::cout << "  Position after " << moveStr << ":" << std::endl;
                Board debugBoard = board;
                Board::MoveState debugState;
                debugBoard.makeMove(move, debugState);
                std::cout << "  FEN: " << debugBoard.getFEN() << std::endl;
                debugBoard.unmakeMove(debugState);
            }
        }
    }
    
    return 0;
}
