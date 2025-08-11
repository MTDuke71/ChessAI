#include <iostream>
#include <chrono>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "Perft.h"

class FastPerftDebug {
private:
    FastMoveGenerator generator;
    // Remove the moveList member variable - we'll use local variables instead
    
public:
    uint64_t perftSimple(Board& board, int depth, bool isWhite) {
        if (depth == 0) return 1ULL;
        
        FastMoveGenerator::MoveList moveList; // Local variable for each call
        generator.generateLegalMoves(board, isWhite, moveList);
        
        if (depth == 1) {
            return moveList.count;
        }
        
        uint64_t nodes = 0ULL;
        for (int i = 0; i < moveList.count; ++i) {
            auto move = moveList.moves[i];
            Board::MoveState state;
            
            std::string moveStr = move.toAlgebraic();
            board.makeMove(moveStr, state);
            nodes += perftSimple(board, depth - 1, !isWhite);
            board.unmakeMove(state);
        }
        return nodes;
    }
    
    uint64_t perftDivideDebug(Board& board, int depth, bool isWhite) {
        FastMoveGenerator::MoveList moveList; // Local variable here too
        generator.generateLegalMoves(board, isWhite, moveList);
        uint64_t total = 0ULL;
        
        std::cout << "Moves at depth " << depth << " for " << (isWhite ? "WHITE" : "BLACK") << ":\n";
        for (int i = 0; i < std::min(3, moveList.count); ++i) { // Only show first 3 moves for debugging
            auto move = moveList.moves[i];
            Board::MoveState state;
            
            std::string moveStr = move.toAlgebraic();
            std::cout << "Making move: " << moveStr << "\n";
            board.makeMove(moveStr, state);
            
            // Show the moves available after this move
            if (depth > 1) {
                FastMoveGenerator::MoveList nextMoves;
                generator.generateLegalMoves(board, !isWhite, nextMoves);
                std::cout << "  Next moves for " << (!isWhite ? "WHITE" : "BLACK") << " (" << nextMoves.count << " moves):\n";
                for (int j = 0; j < std::min(5, nextMoves.count); ++j) {
                    std::cout << "    " << nextMoves.moves[j].toAlgebraic() << "\n";
                }
            }
            
            uint64_t nodes = perftSimple(board, depth - 1, !isWhite);
            total += nodes;
            std::cout << moveStr << ": " << nodes << "\n";
            board.unmakeMove(state);
        }
        
        return total;
    }
};

int main() {
    std::cout << "FastMoveGenerator Perft Debug\n";
    std::cout << "=============================\n\n";
    
    Board board;
    FastPerftDebug perft;
    
    // Test depth 2 and compare with expected
    std::cout << "Testing FastMoveGenerator perft:\n";
    uint64_t fastNodes2 = perft.perftSimple(board, 2, true);
    std::cout << "Fast Perft depth 2: " << fastNodes2 << " (expected: 400)\n\n";
    
    // Test original perft for comparison
    MoveGenerator originalGen;
    double ms;
    uint64_t originalNodes2 = ::perft(board, originalGen, 2, ms);
    std::cout << "Original Perft depth 2: " << originalNodes2 << " (expected: 400)\n\n";
    
    if (fastNodes2 != 400) {
        std::cout << "FastMoveGenerator divide at depth 2:\n";
        uint64_t divideNodes = perft.perftDivideDebug(board, 2, true);
        std::cout << "Total: " << divideNodes << "\n";
    }
    
    return 0;
}
