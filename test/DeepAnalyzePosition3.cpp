#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>
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

void analyzeMoveDetailed(Board& board, const std::string& moveStr, int maxDepth) {
    std::cout << "\n=== Detailed Analysis of " << moveStr << " ===\n";
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, true);
    
    uint16_t targetMove = 0;
    for (uint16_t move : moves) {
        std::string decoded = decodeMove(move);
        std::string converted = decoded;
        if (decoded == "O-O") converted = "e1g1";
        else if (decoded.find('-') != std::string::npos) {
            auto dash = decoded.find('-');
            converted = decoded.substr(0, dash) + decoded.substr(dash + 1);
        }
        
        if (converted == moveStr) {
            targetMove = move;
            break;
        }
    }
    
    if (targetMove == 0) {
        std::cout << "Move not found!\n";
        return;
    }
    
    Board tempBoard = board;
    Board::MoveState state;
    tempBoard.makeMove(targetMove, state);
    
    std::cout << "Position after " << moveStr << ":\n";
    tempBoard.printBoard();
    std::cout << "FEN: " << tempBoard.getFEN() << "\n\n";
    
    // Show perft for different depths
    for (int d = 1; d <= maxDepth; d++) {
        uint64_t result = perft(tempBoard, d);
        std::cout << "Perft D" << d << ": " << result << "\n";
    }
    
    // If this is a significant depth, show divide for depth 1
    if (maxDepth >= 3) {
        std::cout << "\nDivide (depth 1 from this position):\n";
        auto nextMoves = gen.generateLegalMoves(tempBoard, tempBoard.isWhiteToMove());
        for (uint16_t move : nextMoves) {
            Board tempBoard2 = tempBoard;
            Board::MoveState state2;
            tempBoard2.makeMove(move, state2);
            uint64_t count = perft(tempBoard2, maxDepth - 2);
            std::cout << "  " << decodeMove(move) << ": " << count << "\n";
            tempBoard2.unmakeMove(state2);
        }
    }
    
    tempBoard.unmakeMove(state);
}

int main() {
    std::string fen = "4k3/8/8/8/8/8/8/4K2R w K - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== Position 3 Detailed Analysis ===\n";
    std::cout << "FEN: " << fen << "\n";
    board.printBoard();
    
    // Analyze the moves with biggest differences
    analyzeMoveDetailed(board, "e1e2", 5);  // -418 difference
    analyzeMoveDetailed(board, "e1d2", 5);  // -256 difference  
    analyzeMoveDetailed(board, "h1h8", 5);  // -118 difference
    
    return 0;
}
