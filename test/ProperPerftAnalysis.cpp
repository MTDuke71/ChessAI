#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <algorithm>

class ProperPerftAnalysis {
public:
    static void analyzeE2D1Perft() {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        
        Board board;
        board.loadFEN(fen);
        
        std::cout << "=== PROPER PERFT ANALYSIS FOR E2-D1 ===\n";
        std::cout << "Expected depth 3 nodes after e2-d1: 2218\n";
        
        // Make the e2-d1 move
        Board::MoveState state;
        board.makeMove("e2-d1", state);
        
        std::cout << "Position after e2-d1:\n";
        board.printBoard();
        std::cout << "\n";
        
        // Now do proper perft analysis (checking for king in check)
        MoveGenerator gen;
        auto blackMoves = gen.generateAllMoves(board, false);
        
        std::cout << "Generated " << blackMoves.size() << " black moves\n";
        
        uint64_t totalDepth2Nodes = 0;
        std::vector<std::pair<std::string, uint64_t>> moveAnalysis;
        
        for (auto blackMove : blackMoves) {
            std::string blackMoveStr = decodeMove(blackMove);
            
            Board copy = board;
            Board::MoveState blackState;
            copy.makeMove(blackMoveStr, blackState);
            
            // Check if this move leaves the king in check (illegal move)
            if (gen.isKingInCheck(copy, false)) {
                // This move is illegal, skip it
                continue;
            }
            
            // Count white's legal responses (proper perft style)
            auto whiteMoves = gen.generateAllMoves(copy, true);
            uint64_t legalWhiteResponses = 0;
            
            for (auto whiteMove : whiteMoves) {
                std::string whiteMoveStr = decodeMove(whiteMove);
                
                Board whiteTestBoard = copy;
                Board::MoveState whiteState;
                whiteTestBoard.makeMove(whiteMoveStr, whiteState);
                
                // Check if white's move leaves white king in check (illegal)
                if (!gen.isKingInCheck(whiteTestBoard, false)) {
                    legalWhiteResponses++;
                }
                
                whiteTestBoard.unmakeMove(whiteState);
            }
            
            totalDepth2Nodes += legalWhiteResponses;
            moveAnalysis.push_back({blackMoveStr, legalWhiteResponses});
        }
        
        std::cout << "Total legal depth 2 nodes after e2-d1: " << totalDepth2Nodes << "\n";
        std::cout << "Expected: 2218\n";
        std::cout << "Difference: " << (int64_t)totalDepth2Nodes - 2218 << "\n\n";
        
        // Show legal black moves and their white response counts
        std::cout << "Legal black moves after e2-d1 and their white response counts:\n";
        std::sort(moveAnalysis.begin(), moveAnalysis.end(), 
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        for (const auto& [move, count] : moveAnalysis) {
            std::cout << move << ": " << count << " white responses\n";
        }
        
        // Let's also test the built-in perft function
        std::cout << "\n=== BUILT-IN PERFT TEST ===\n";
        double ms;
        uint64_t builtinResult = perft(board, gen, 2, ms);
        std::cout << "Built-in perft(2) after e2-d1: " << builtinResult << "\n";
        std::cout << "Time: " << ms << " ms\n";
    }
};

int main() {
    ProperPerftAnalysis::analyzeE2D1Perft();
    return 0;
}
