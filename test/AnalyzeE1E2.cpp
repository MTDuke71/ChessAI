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
    if (move == "O-O") return "e8g8";
    if (move == "O-O-O") return "e8c8";
    auto dash = move.find('-');
    if (dash != std::string::npos) {
        return move.substr(0, dash) + move.substr(dash + 1);
    }
    return move;
}

int main() {
    // Position after e1e2: 4k3/8/8/8/8/8/4K3/7R b - - 1 2
    std::string fen = "4k3/8/8/8/8/8/4K3/7R b - - 1 2";
    
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== Analyzing position after e1e2 ===\n";
    std::cout << "FEN: " << fen << "\n\n";
    
    board.printBoard();
    
    // Expected breakdown for black's responses
    std::map<std::string, uint64_t> expected = {
        {"e8d7", 19713},
        {"e8d8", 12215},
        {"e8e7", 19612},
        {"e8f7", 18493},
        {"e8f8", 11451}
    };
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, false); // Black to move
    
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
        std::string rawMoveStr = decodeMove(move);
        std::string moveStr = convertMoveFormat(rawMoveStr);
        
        Board tempBoard = board;
        Board::MoveState state;
        tempBoard.makeMove(move, state);
        uint64_t count = perft(tempBoard, 4); // Depth 4 since we're already 1 move deep
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
    }
    
    std::cout << std::string(50, '-') << "\n";
    std::cout << std::left << std::setw(8) << "TOTAL"
              << std::setw(12) << totalActual
              << std::setw(12) << totalExpected
              << std::setw(10) << ((int64_t)totalActual - (int64_t)totalExpected)
              << "\n\n";
    
    // Now let's analyze the move with largest discrepancy
    for (uint16_t move : moves) {
        std::string rawMoveStr = decodeMove(move);
        std::string moveStr = convertMoveFormat(rawMoveStr);
        auto it = expected.find(moveStr);
        if (it != expected.end()) {
            Board tempBoard = board;
            Board::MoveState state;
            tempBoard.makeMove(move, state);
            uint64_t count = perft(tempBoard, 4);
            int64_t diff = (int64_t)count - (int64_t)it->second;
            
            if (std::abs(diff) > 50) { // Show moves with significant discrepancy
                std::cout << "\n--- Analyzing move: " << moveStr << " (raw: " << rawMoveStr << ", diff: " << diff << ") ---\n";
                tempBoard.printBoard();
                std::cout << "FEN: " << tempBoard.getFEN() << "\n";
                
                // Show perft breakdown for this position
                std::cout << "Perft breakdown for depth 1-3:\n";
                for (int d = 1; d <= 3; d++) {
                    uint64_t result = perft(tempBoard, d);
                    std::cout << "  D" << d << ": " << result << "\n";
                }
                
                // Show first few moves from this position
                auto nextMoves = gen.generateLegalMoves(tempBoard, tempBoard.isWhiteToMove());
                std::cout << "Next moves (" << nextMoves.size() << " total):\n";
                for (size_t i = 0; i < std::min((size_t)10, nextMoves.size()); i++) {
                    std::cout << "  " << decodeMove(nextMoves[i]) << "\n";
                }
            }
            tempBoard.unmakeMove(state);
        }
    }
    
    return 0;
}
