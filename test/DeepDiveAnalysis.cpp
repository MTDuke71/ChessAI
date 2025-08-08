#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <iomanip>
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

uint64_t perftDivide(Board& board, int depth) {
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, board.isWhiteToMove());
    
    uint64_t totalNodes = 0;
    
    std::cout << "Moves from position (" << moves.size() << " total):\n";
    for (uint16_t move : moves) {
        std::string moveStr = decodeMove(move);
        Board::MoveState state;
        board.makeMove(move, state);
        uint64_t count = perft(board, depth - 1);
        totalNodes += count;
        std::cout << moveStr << ": " << count << "\n";
        board.unmakeMove(state);
    }
    std::cout << "Total: " << totalNodes << "\n\n";
    return totalNodes;
}

int main() {
    // Start from position after e1e2, e8d7
    // FEN: 8/3k4/8/8/8/8/4K3/7R w - - 2 2
    std::string fen = "8/3k4/8/8/8/8/4K3/7R w - - 2 2";
    
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== Deep Dive: e1e2, e8d7 Position ===\n";
    std::cout << "FEN: " << fen << "\n\n";
    
    board.printBoard();
    
    std::cout << "Expected at depth 3: 3260 (from our calculation)\n";
    std::cout << "Let's see what we get:\n\n";
    
    // Show perft divide for depth 1, 2, and 3
    for (int depth = 1; depth <= 3; depth++) {
        std::cout << "=== DEPTH " << depth << " ===\n";
        uint64_t result = perftDivide(board, depth);
        std::cout << "Depth " << depth << " result: " << result << "\n\n";
    }
    
    return 0;
}
