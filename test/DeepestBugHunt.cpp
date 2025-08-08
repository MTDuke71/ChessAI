#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>
#include <map>

int main() {
    Board board;
    board.loadFEN("4R3/8/3k4/8/8/8/8/4K3 w - - 4 3");
    
    std::cout << "=== DEEPEST BUG ANALYSIS: e7-d6 Position ===" << std::endl;
    std::cout << "Position: 4R3/8/3k4/8/8/8/8/4K3 w - - 4 3" << std::endl;
    std::cout << "This position is missing 5 nodes (Expected: 104, Getting: 99)" << std::endl;
    std::cout << std::endl;
    
    board.printBoard();
    
    // Expected results from user's data (Depth 2)
    std::map<std::string, int> expected = {
        {"e1d1", 5}, {"e1d2", 5}, {"e1e2", 5}, {"e1f1", 5}, {"e1f2", 5},
        {"e8a8", 8}, {"e8b8", 8}, {"e8c8", 5}, {"e8d8", 6}, {"e8e2", 5},
        {"e8e3", 5}, {"e8e4", 5}, {"e8e5", 4}, {"e8e6", 5}, {"e8e7", 4},
        {"e8f8", 8}, {"e8g8", 8}, {"e8h8", 8}
    };
    int expectedTotal = 104;
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, true); // White to move
    
    int totalActual = 0;
    int totalExpected = 0;
    int totalDelta = 0;
    
    std::cout << "=== DEPTH 2 BUG HUNT ===" << std::endl;
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
        int actual = perft(copy, gen, 1);  // Depth 1 from this position
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
    std::cout << "- This MUST equal -5 to match e7-d6 discrepancy" << std::endl;
    
    if (totalDelta == -5) {
        std::cout << "✅ CONFIRMED: This is the source of the 5-node bug!" << std::endl;
    } else {
        std::cout << "❌ MISMATCH: Expected -5 delta, got " << totalDelta << std::endl;
    }
    
    if (totalDelta != 0) {
        std::cout << std::endl;
        std::cout << "🐛 EXACT BUG LOCATIONS:" << std::endl;
        std::cout << "Missing or incorrect moves:" << std::endl;
        
        for (const auto& move : moves) {
            std::string moveStr = decodeMove(move);
            std::string key = moveStr;
            if (key.find('-') != std::string::npos) {
                key.erase(key.find('-'), 1);
            }
            
            Board copy = board;
            Board::MoveState state;
            copy.makeMove(move, state);
            int actual = perft(copy, gen, 1);
            copy.unmakeMove(state);
            
            int exp = expected.count(key) ? expected[key] : 0;
            int delta = actual - exp;
            
            if (delta != 0) {
                std::cout << "- " << moveStr << ": Expected " << exp << ", Got " << actual 
                         << " (" << delta << " nodes)" << std::endl;
                
                // Show the detailed position for debugging
                Board debugBoard = board;
                Board::MoveState debugState;
                debugBoard.makeMove(move, debugState);
                std::cout << "  After " << moveStr << ": " << debugBoard.getFEN() << std::endl;
                
                // Show what moves are available from this position
                auto nextMoves = gen.generateLegalMoves(debugBoard, !debugBoard.isWhiteToMove());
                std::cout << "  Next moves (" << nextMoves.size() << "): ";
                for (size_t i = 0; i < nextMoves.size() && i < 5; ++i) {
                    std::cout << decodeMove(nextMoves[i]) << " ";
                }
                if (nextMoves.size() > 5) std::cout << "...";
                std::cout << std::endl;
                
                debugBoard.unmakeMove(debugState);
            }
        }
        
        // Check if there are any expected moves we're not generating
        std::cout << std::endl;
        std::cout << "Checking for missing moves:" << std::endl;
        for (const auto& pair : expected) {
            bool found = false;
            for (const auto& move : moves) {
                std::string moveStr = decodeMove(move);
                std::string key = moveStr;
                if (key.find('-') != std::string::npos) {
                    key.erase(key.find('-'), 1);
                }
                if (key == pair.first) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cout << "- MISSING MOVE: " << pair.first << " (Expected " << pair.second << " nodes)" << std::endl;
            }
        }
    }
    
    return 0;
}
