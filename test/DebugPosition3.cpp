#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include "../src/PrintMoves.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <string>

uint64_t perft(Board& board, int depth) {
    if (depth == 0) return 1;
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, board.isWhiteToMove());
    
    uint64_t nodes = 0;
    for (uint16_t move : moves) {
        Board::MoveState state;
        board.makeMove(move, state);
        nodes += perft(board, depth - 1);
        board.unmakeMove(state);
    }
    return nodes;
}

int main() {
    // Position 3: 4k3/8/8/8/8/8/8/4K2R w K - 0 1
    std::string fen = "4k3/8/8/8/8/8/8/4K2R w K - 0 1";
    
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== Position 3 Debug ===\n";
    std::cout << "FEN: " << fen << "\n\n";
    
    board.printBoard();
    
    MoveGenerator gen;
    
    // Expected results from your data
    std::map<std::string, uint64_t> expected = {
        {"e1d1", 47643},
        {"e1d2", 81423},
        {"e1e2", 81484},
        {"e1f1", 42182},
        {"e1f2", 79592},
        {"e1g1", 26553},
        {"h1f1", 22226},
        {"h1g1", 39089},
        {"h1h2", 60485},
        {"h1h3", 63965},
        {"h1h4", 63965},
        {"h1h5", 61436},
        {"h1h6", 48719},
        {"h1h7", 9553},
        {"h1h8", 36328}
    };
    
    auto moves = gen.generateLegalMoves(board, true);
    
    std::cout << "Legal moves from position: " << moves.size() << "\n\n";
    
    uint64_t totalActual = 0;
    uint64_t totalExpected = 0;
    
    std::cout << std::left << std::setw(8) << "Move" 
              << std::setw(12) << "Actual" 
              << std::setw(12) << "Expected" 
              << std::setw(10) << "Diff" 
              << "Status\n";
    std::cout << std::string(50, '-') << "\n";
    
    for (uint16_t move : moves) {
        std::string moveStr = decodeMove(move);
        
        Board tempBoard = board;
        Board::MoveState state;
        tempBoard.makeMove(move, state);
        uint64_t count = perft(tempBoard, 5);
        totalActual += count;
        
        auto it = expected.find(moveStr);
        if (it != expected.end()) {
            uint64_t exp = it->second;
            totalExpected += exp;
            int64_t diff = (int64_t)count - (int64_t)exp;
            
            std::cout << std::left << std::setw(8) << moveStr
                      << std::setw(12) << count
                      << std::setw(12) << exp
                      << std::setw(10) << diff;
            
            if (diff == 0) {
                std::cout << "✓";
            } else {
                std::cout << "✗ DIFF: " << diff;
            }
            std::cout << "\n";
        } else {
            std::cout << std::left << std::setw(8) << moveStr
                      << std::setw(12) << count
                      << std::setw(12) << "MISSING"
                      << std::setw(10) << "?"
                      << "? NOT IN EXPECTED\n";
        }
    }
    
    // Check for expected moves we didn't generate
    for (const auto& [moveStr, exp] : expected) {
        bool found = false;
        for (uint16_t move : moves) {
            if (decodeMove(move) == moveStr) {
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << std::left << std::setw(8) << moveStr
                      << std::setw(12) << "MISSING"
                      << std::setw(12) << exp
                      << std::setw(10) << "?"
                      << "? NOT GENERATED\n";
            totalExpected += exp;
        }
    }
    
    std::cout << std::string(50, '-') << "\n";
    std::cout << std::left << std::setw(8) << "TOTAL"
              << std::setw(12) << totalActual
              << std::setw(12) << totalExpected
              << std::setw(10) << ((int64_t)totalActual - (int64_t)totalExpected)
              << "\n\n";
    
    // Let's also check individual move generation for any suspicious moves
    std::cout << "=== Detailed Move Analysis ===\n";
    for (uint16_t move : moves) {
        std::string moveStr = decodeMove(move);
        auto it = expected.find(moveStr);
        if (it != expected.end()) {
            Board tempBoard = board;
            Board::MoveState state;
            tempBoard.makeMove(move, state);
            uint64_t count = perft(tempBoard, 5);
            int64_t diff = (int64_t)count - (int64_t)it->second;
            
            if (diff != 0) {
                std::cout << "\n--- Analyzing move: " << moveStr << " (diff: " << diff << ") ---\n";
                Board tempBoard = board;
                Board::MoveState state;
                tempBoard.makeMove(move, state);
                tempBoard.printBoard();
                std::cout << "FEN: " << tempBoard.getFEN() << "\n";
                
                // Show perft breakdown for this position
                std::cout << "Perft breakdown for depth 1-3:\n";
                for (int d = 1; d <= 3; d++) {
                    uint64_t result = perft(tempBoard, d);
                    std::cout << "  D" << d << ": " << result << "\n";
                }
                tempBoard.unmakeMove(state);
            }
        }
    }
    
    return 0;
}
