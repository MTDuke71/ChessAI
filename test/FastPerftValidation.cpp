#include <iostream>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "Perft.h"
#include "MoveGenerator.h"

int main() {
    std::cout << "FastMoveGenerator Perft Validation\n";
    std::cout << "===================================\n\n";
    
    Board board;
    FastMoveGenerator fastGen;
    MoveGenerator originalGen;
    
    for (int depth = 1; depth <= 6; ++depth) {
        std::cout << "Testing depth " << depth << ":\n";
        
        // FastMoveGenerator test
        class FastPerftTester {
        private:
            FastMoveGenerator& generator;
        public:
            FastPerftTester(FastMoveGenerator& gen) : generator(gen) {}
            
            uint64_t perft(Board& board, int depth, bool isWhite) {
                if (depth == 0) return 1ULL;
                
                FastMoveGenerator::MoveList moveList;
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
                    nodes += perft(board, depth - 1, !isWhite);
                    board.unmakeMove(state);
                }
                return nodes;
            }
        };
        
        FastPerftTester tester(fastGen);
        uint64_t fastNodes = tester.perft(board, depth, true);
        uint64_t originalNodes = perft(board, originalGen, depth); // Use function instead of class
        
        std::cout << "  FastMoveGenerator: " << fastNodes << "\n";
        std::cout << "  Original Perft:    " << originalNodes << "\n";
        
        if (fastNodes == originalNodes) {
            std::cout << "  ✅ MATCH\n";
        } else {
            std::cout << "  ❌ MISMATCH!\n";
        }
        std::cout << "\n";
    }
    
    return 0;
}
