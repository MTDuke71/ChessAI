#include "../src/Board.h"
#include "../src/MoveGenerator.h"
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

std::string convertMoveFormat(const std::string& move) {
    // Convert from "h1-f1" or "O-O" format to "h1f1" or "e1g1" format
    if (move == "O-O") {
        return "e1g1";  // Convert castling to king move notation
    }
    if (move == "O-O-O") {
        return "e1c1";  // Convert queenside castling to king move notation
    }
    
    // Remove the dash from regular moves
    auto dash = move.find('-');
    if (dash != std::string::npos) {
        return move.substr(0, dash) + move.substr(dash + 1);
    }
    return move;
}

int main() {
    // Position 3: 4k3/8/8/8/8/8/8/4K2R w K - 0 1
    std::string fen = "4k3/8/8/8/8/8/8/4K2R w K - 0 1";
    
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== Position 3: e1e2 Move Debug ===\n";
    std::cout << "Initial FEN: " << fen << "\n\n";
    
    board.printBoard();
    
    // Make the e1e2 move
    uint16_t e1e2Move = encodeMove("e1-e2");
    Board::MoveState state;
    board.makeMove(e1e2Move, state);
    
    std::cout << "After e1e2:\n";
    board.printBoard();
    std::cout << "FEN: " << board.getFEN() << "\n\n";
    
    // Expected results for e1e2 at depth 5
    std::map<std::string, uint64_t> expected = {
        {"e8d7", 19713},
        {"e8d8", 12215},
        {"e8e7", 19612},
        {"e8f7", 18493},
        {"e8f8", 11451}
    };
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, board.isWhiteToMove());
    
    std::cout << "Legal moves from position (should be 5 black king moves): " << moves.size() << "\n\n";
    
    uint64_t totalActual = 0;
    uint64_t totalExpected = 0;
    
    std::cout << std::left << std::setw(8) << "Move" 
              << std::setw(12) << "Actual" 
              << std::setw(12) << "Expected" 
              << std::setw(10) << "Diff" 
              << "Status\n";
    std::cout << std::string(50, '-') << "\n";
    
    for (uint16_t move : moves) {
        std::string rawMoveStr = decodeMove(move);
        std::string moveStr = convertMoveFormat(rawMoveStr);
        
        Board tempBoard = board;
        Board::MoveState tempState;
        tempBoard.makeMove(move, tempState);
        uint64_t count = perft(tempBoard, 4);  // depth 4 since we're already 1 move deep
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
                      << "? NOT IN EXPECTED (" << rawMoveStr << ")\n";
        }
        tempBoard.unmakeMove(tempState);
    }
    
    // Check for expected moves we didn't generate
    for (const auto& [moveStr, exp] : expected) {
        bool found = false;
        for (uint16_t move : moves) {
            if (convertMoveFormat(decodeMove(move)) == moveStr) {
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
    
    // Analyze any discrepancies in detail
    std::cout << "=== Detailed Analysis ===\n";
    for (uint16_t move : moves) {
        std::string rawMoveStr = decodeMove(move);
        std::string moveStr = convertMoveFormat(rawMoveStr);
        auto it = expected.find(moveStr);
        if (it != expected.end()) {
            Board tempBoard = board;
            Board::MoveState tempState;
            tempBoard.makeMove(move, tempState);
            uint64_t count = perft(tempBoard, 4);
            int64_t diff = (int64_t)count - (int64_t)it->second;
            
            if (diff != 0) {
                std::cout << "\n--- Analyzing move: " << moveStr << " (raw: " << rawMoveStr << ", diff: " << diff << ") ---\n";
                tempBoard.printBoard();
                std::cout << "FEN: " << tempBoard.getFEN() << "\n";
                
                // Show perft breakdown for this position
                std::cout << "Perft breakdown for depth 1-3:\n";
                for (int d = 1; d <= 3; d++) {
                    uint64_t result = perft(tempBoard, d);
                    std::cout << "  D" << d << ": " << result << "\n";
                }
            }
            tempBoard.unmakeMove(tempState);
        }
    }
    
    board.unmakeMove(state);
    return 0;
}
