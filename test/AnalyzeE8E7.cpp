#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>
#include <map>

int main() {
    Board board;
    board.loadFEN("4k2R/4K3/8/8/8/8/8/8 w - - 2 2");
    
    std::cout << "=== ANALYSIS: e8-e7 Position ===" << std::endl;
    std::cout << "Position: 4k2R/4K3/8/8/8/8/8/8 w - - 2 2" << std::endl;
    std::cout << "This is the position after h1-h8, e8-e7 from original Position 3" << std::endl;
    std::cout << std::endl;
    
    board.printBoard();
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, true); // White to move
    
    std::cout << "=== MOVE GENERATION ANALYSIS ===" << std::endl;
    std::cout << "White to move (" << moves.size() << " legal moves)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "All legal moves from this position:" << std::endl;
    for (size_t i = 0; i < moves.size(); ++i) {
        std::string moveStr = decodeMove(moves[i]);
        std::cout << i+1 << ". " << moveStr << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== DEPTH ANALYSIS ===" << std::endl;
    
    // Calculate perft at different depths
    for (int depth = 1; depth <= 4; ++depth) {
        int nodes = perft(board, gen, depth);
        std::cout << "Depth " << depth << ": " << nodes << " nodes" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Expected at depth 4: 12331 nodes" << std::endl;
    std::cout << "Actual at depth 4: " << perft(board, gen, 4) << " nodes" << std::endl;
    std::cout << "Delta: " << (perft(board, gen, 4) - 12331) << " nodes" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== MOVE-BY-MOVE BREAKDOWN ===" << std::endl;
    std::cout << "Move     | Depth 3 Nodes" << std::endl;
    std::cout << "---------|---------------" << std::endl;
    
    int totalNodes = 0;
    for (const auto& move : moves) {
        std::string moveStr = decodeMove(move);
        
        Board copy = board;
        Board::MoveState state;
        copy.makeMove(move, state);
        int nodes = perft(copy, gen, 3);
        copy.unmakeMove(state);
        
        totalNodes += nodes;
        printf("%-8s | %6d\n", moveStr.c_str(), nodes);
    }
    
    std::cout << "---------|---------------" << std::endl;
    printf("TOTAL    | %6d\n", totalNodes);
    
    return 0;
}
