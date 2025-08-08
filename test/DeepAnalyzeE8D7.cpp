#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <iomanip>
#include <set>

uint64_t perftWithMoveList(Board& board, int depth, std::set<std::string>& allMoves) {
    if (depth == 0) return 1;
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, board.isWhiteToMove());
    
    uint64_t nodes = 0;
    for (uint16_t move : moves) {
        if (depth == 1) {
            allMoves.insert(decodeMove(move));
        }
        Board::MoveState state;
        board.makeMove(move, state);
        nodes += perftWithMoveList(board, depth - 1, allMoves);
        board.unmakeMove(state);
    }
    return nodes;
}

int main() {
    // Let's analyze the position after e8d7 (which had -100 diff)
    // FEN: 8/3k4/8/8/8/8/4K3/7R w - - 2 2
    std::string fen = "8/3k4/8/8/8/8/4K3/7R w - - 2 2";
    
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== Deep Analysis: Position after e1e2, e8d7 ===\n";
    std::cout << "FEN: " << fen << "\n\n";
    
    board.printBoard();
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, true); // White to move
    
    std::cout << "White's moves from this position (" << moves.size() << " total):\n";
    
    std::set<std::string> depth2Moves;
    uint64_t totalNodes = 0;
    
    for (uint16_t move : moves) {
        std::string moveStr = decodeMove(move);
        Board tempBoard = board;
        Board::MoveState state;
        tempBoard.makeMove(move, state);
        
        std::set<std::string> movesFromHere;
        uint64_t count = perftWithMoveList(tempBoard, 2, movesFromHere);
        totalNodes += count;
        
        std::cout << std::left << std::setw(8) << moveStr 
                  << " -> " << std::setw(6) << count 
                  << " nodes (depth 2)\n";
        
        // Add all depth-2 moves to our set
        for (const auto& m : movesFromHere) {
            depth2Moves.insert(moveStr + " " + m);
        }
        
        tempBoard.unmakeMove(state);
    }
    
    std::cout << "\nTotal nodes at depth 3: " << totalNodes << " (expected ~3360)\n";
    std::cout << "Discrepancy: " << (3260 - totalNodes) << " nodes\n\n";
    
    // Let's also check a position that should give 3360 nodes
    // We can create a simple reference position and see what differs
    std::cout << "=== Unique depth-2 move sequences found: " << depth2Moves.size() << " ===\n";
    
    // Show first 20 move sequences for analysis
    int count = 0;
    for (const auto& moveSeq : depth2Moves) {
        if (count++ < 20) {
            std::cout << moveSeq << "\n";
        }
    }
    if (depth2Moves.size() > 20) {
        std::cout << "... (" << (depth2Moves.size() - 20) << " more)\n";
    }
    
    return 0;
}
