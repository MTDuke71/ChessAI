#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

uint64_t perftWithLogging(Board& board, int depth, int maxDepth = 3) {
    if (depth == 0) return 1;
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, board.isWhiteToMove());
    
    if (depth == maxDepth) {
        std::cout << "Position (depth " << depth << "): " << board.getFEN() << "\n";
        std::cout << "Legal moves: " << moves.size() << "\n";
        for (uint16_t move : moves) {
            std::cout << "  " << decodeMove(move) << "\n";
        }
        std::cout << "\n";
    }
    
    uint64_t nodes = 0;
    for (uint16_t move : moves) {
        Board::MoveState state;
        board.makeMove(move, state);
        nodes += perftWithLogging(board, depth - 1, maxDepth);
        board.unmakeMove(state);
    }
    return nodes;
}

int main() {
    // Let's carefully trace the first few moves to see if we can spot the issue
    std::string fen = "4k3/8/8/8/8/8/8/4K2R w K - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== Systematic Analysis ===\n";
    std::cout << "Starting position: " << fen << "\n\n";
    
    // Make e1e2
    board.makeMove("e1-e2");
    std::cout << "After e1e2: " << board.getFEN() << "\n";
    
    // Make e8d7
    board.makeMove("e8-d7");
    std::cout << "After e8d7: " << board.getFEN() << "\n\n";
    
    // Now let's see what moves are available and trace one level deep
    std::cout << "Analyzing this position at depth 2...\n\n";
    uint64_t result = perftWithLogging(board, 2, 2);
    std::cout << "Total nodes at depth 2: " << result << "\n";
    
    return 0;
}
