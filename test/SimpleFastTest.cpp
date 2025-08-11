#include <iostream>
#include <chrono>
#include "Board.h"
#include "FastMoveGenerator.h"

class SimpleFastPerft {
private:
    FastMoveGenerator generator;
    
public:
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

int main() {
    std::cout << "Simple FastMoveGenerator Test\n";
    std::cout << "=============================\n\n";
    
    Board board;
    SimpleFastPerft tester;
    
    auto start = std::chrono::high_resolution_clock::now();
    uint64_t nodes = tester.perft(board, 5, true);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    double nps = (nodes * 1000.0) / duration.count();
    
    std::cout << "Depth: 5\n";
    std::cout << "Nodes: " << nodes << "\n";
    std::cout << "Time: " << duration.count() << " ms\n";
    std::cout << "NPS: " << static_cast<uint64_t>(nps) << " nodes/second\n\n";
    
    std::cout << "Expected depth 5: 4,865,609 nodes\n";
    if (nodes == 4865609) {
        std::cout << "✅ PERFECT MATCH!\n";
    } else {
        std::cout << "❌ Difference: " << (int64_t)nodes - 4865609 << " nodes\n";
    }
    
    return 0;
}
