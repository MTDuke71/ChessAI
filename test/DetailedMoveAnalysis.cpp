#include <iostream>
#include <vector>
#include <string>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h"
#include "Perft.h"

uint64_t fastPerftDetailed(Board& board, FastMoveGenerator& gen, int depth, bool isWhite, std::vector<std::pair<std::string, uint64_t>>& moveResults) {
    if (depth == 0) return 1ULL;
    
    FastMoveGenerator::MoveList moveList;
    gen.generateLegalMoves(board, isWhite, moveList);
    
    if (depth == 1) {
        return moveList.count;
    }
    
    uint64_t totalNodes = 0ULL;
    for (int i = 0; i < moveList.count; ++i) {
        auto move = moveList.moves[i];
        Board::MoveState state;
        
        std::string moveStr = move.toAlgebraic();
        board.makeMove(moveStr, state);
        uint64_t childNodes = fastPerftDetailed(board, gen, depth - 1, !isWhite, moveResults);
        totalNodes += childNodes;
        
        if (depth == 5) {
            moveResults.push_back({moveStr, childNodes});
        }
        
        board.unmakeMove(state);
    }
    return totalNodes;
}

uint64_t originalPerftDetailed(Board& board, MoveGenerator& gen, int depth, bool isWhite, std::vector<std::pair<std::string, uint64_t>>& moveResults) {
    if (depth == 0) return 1ULL;
    
    std::vector<std::string> moves;
    gen.generateAllLegalMoves(board, moves, isWhite);
    
    if (depth == 1) {
        return moves.size();
    }
    
    uint64_t totalNodes = 0ULL;
    for (const auto& moveStr : moves) {
        Board::MoveState state;
        
        board.makeMove(moveStr, state);
        uint64_t childNodes = originalPerftDetailed(board, gen, depth - 1, !isWhite, moveResults);
        totalNodes += childNodes;
        
        if (depth == 5) {
            moveResults.push_back({moveStr, childNodes});
        }
        
        board.unmakeMove(state);
    }
    return totalNodes;
}

int main() {
    std::cout << "Detailed Move-by-Move Comparison at Depth 5\n";
    std::cout << "===========================================\n\n";
    
    Board board1, board2;
    FastMoveGenerator fastGen;
    MoveGenerator originalGen;
    
    std::vector<std::pair<std::string, uint64_t>> fastResults;
    std::vector<std::pair<std::string, uint64_t>> originalResults;
    
    std::cout << "Running FastMoveGenerator perft(5)...\n";
    uint64_t fastTotal = fastPerftDetailed(board1, fastGen, 5, true, fastResults);
    
    std::cout << "Running Original perft(5)...\n";
    uint64_t originalTotal = originalPerftDetailed(board2, originalGen, 5, true, originalResults);
    
    std::cout << "\nMove-by-move comparison:\n";
    std::cout << "========================\n";
    
    // Compare move by move
    for (size_t i = 0; i < std::min(fastResults.size(), originalResults.size()); ++i) {
        if (fastResults[i].first != originalResults[i].first || fastResults[i].second != originalResults[i].second) {
            std::cout << "DIFFERENCE at move " << i+1 << ":\n";
            std::cout << "  Fast: " << fastResults[i].first << " = " << fastResults[i].second << " nodes\n";
            std::cout << "  Orig: " << originalResults[i].first << " = " << originalResults[i].second << " nodes\n";
            std::cout << "  Diff: " << (int64_t)(fastResults[i].second - originalResults[i].second) << "\n\n";
        }
    }
    
    if (fastResults.size() != originalResults.size()) {
        std::cout << "MOVE COUNT DIFFERENCE:\n";
        std::cout << "  FastMoveGenerator: " << fastResults.size() << " moves\n";
        std::cout << "  Original:          " << originalResults.size() << " moves\n\n";
    }
    
    std::cout << "Totals:\n";
    std::cout << "  FastMoveGenerator: " << fastTotal << " nodes\n";
    std::cout << "  Original:          " << originalTotal << " nodes\n";
    std::cout << "  Difference:        " << (int64_t)(fastTotal - originalTotal) << " nodes\n";
    
    return 0;
}
