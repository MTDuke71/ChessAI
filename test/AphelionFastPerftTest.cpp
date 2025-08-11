#include <iostream>
#include <chrono>
#include "Board.h"
#include "FastMoveGenerator.h"

class FastPerft {
private:
    FastMoveGenerator generator;
    FastMoveGenerator::MoveList moveList;
    
    uint64_t perftRecursive(Board& board, int depth, bool isWhite) {
        if (depth == 0) return 1ULL;
        
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
            nodes += perftRecursive(board, depth - 1, !isWhite);
            board.unmakeMove(state);
        }
        return nodes;
    }
    
public:
    uint64_t perftDivide(Board& board, int depth, bool isWhite) {
        generator.generateLegalMoves(board, isWhite, moveList);
        uint64_t total = 0ULL;
        
        for (int i = 0; i < moveList.count; ++i) {
            auto move = moveList.moves[i];
            Board::MoveState state;
            
            std::string moveStr = move.toAlgebraic();
            board.makeMove(moveStr, state);
            uint64_t nodes = perftRecursive(board, depth - 1, !isWhite);
            total += nodes;
            std::cout << moveStr << ": " << nodes << "\n";
            board.unmakeMove(state);
        }
        
        return total;
    }
};

int main() {
    std::cout << "Aphelion Fast Engine - Perft 6 Test\n";
    std::cout << "====================================\n\n";
    
    Board board;
    FastPerft perft;
    
    std::cout << "Testing optimized Perft on starting position:\n";
    
    const int depth = 6;
    
    std::cout << "\nTesting depth " << depth << ":\n";
    
    auto start = std::chrono::steady_clock::now();
    uint64_t nodes = perft.perftDivide(board, depth, true);
    auto end = std::chrono::steady_clock::now();
    
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    
    std::cout << "\nDepth: " << depth << std::endl;
    std::cout << "Nodes: " << nodes << std::endl;
    std::cout << "Time: " << ms << " ms" << std::endl;
    
    if (ms > 0) {
        std::cout << "NPS: " << (long)(nodes / (ms / 1000.0)) << " nodes/second" << std::endl;
    }
    
    std::cout << "\nAphelion Fast Performance Test Complete.\n";
    std::cout << "\nOptimizations applied:\n";
    std::cout << "- Integer move encoding (vs string moves)\n";
    std::cout << "- Pre-allocated move lists (vs std::vector)\n";
    std::cout << "- Precomputed attack tables\n";
    std::cout << "- Bitboard manipulation optimizations\n";
    std::cout << "- Reduced function call overhead\n";
    std::cout << "- Proper legal move filtering\n";
    
    return 0;
}
