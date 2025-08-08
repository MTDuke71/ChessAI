#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <algorithm>

class CorrectedPerftAnalysis {
public:
    static void analyzeE2D1Perft() {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        
        Board board;
        board.loadFEN(fen);
        
        std::cout << "=== CORRECTED PERFT ANALYSIS FOR E2-D1 ===\n";
        std::cout << "Expected depth 3 nodes after e2-d1: 2218\n";
        
        // Make the e2-d1 move
        Board::MoveState state;
        board.makeMove("e2-d1", state);
        
        std::cout << "Position after e2-d1 (black to move):\n";
        board.printBoard();
        std::cout << "\n";
        
        // Now do perft analysis exactly like the built-in function
        MoveGenerator gen;
        auto blackMoves = gen.generateAllMoves(board, false); // Black to move
        
        std::cout << "Generated " << blackMoves.size() << " black moves\n";
        
        uint64_t totalDepth2Nodes = 0;
        std::vector<std::pair<std::string, uint64_t>> moveAnalysis;
        
        for (auto blackMove : blackMoves) {
            std::string blackMoveStr = decodeMove(blackMove);
            
            Board copy = board;
            Board::MoveState blackState;
            copy.makeMove(blackMoveStr, blackState);
            
            // Check if this move is legal (doesn't leave own king in check)
            // After black moves, we check if black king is in check
            if (gen.isKingInCheck(copy, false)) {
                // This move is illegal, skip it
                continue;
            }
            
            // Count white's legal responses using the same logic as perft
            auto whiteMoves = gen.generateAllMoves(copy, true); // White to move
            uint64_t legalWhiteResponses = 0;
            
            for (auto whiteMove : whiteMoves) {
                std::string whiteMoveStr = decodeMove(whiteMove);
                
                Board whiteTestBoard = copy;
                Board::MoveState whiteState;
                whiteTestBoard.makeMove(whiteMoveStr, whiteState);
                
                // After white moves, check if white king is in check
                if (!gen.isKingInCheck(whiteTestBoard, true)) {
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
        
        // Compare with built-in perft
        std::cout << "=== BUILT-IN PERFT COMPARISON ===\n";
        double ms;
        uint64_t builtinResult = perft(board, gen, 2, ms);
        std::cout << "Built-in perft(2) after e2-d1: " << builtinResult << "\n";
        std::cout << "My calculation: " << totalDepth2Nodes << "\n";
        std::cout << "Difference from built-in: " << (int64_t)totalDepth2Nodes - (int64_t)builtinResult << "\n";
        
        if (totalDepth2Nodes == builtinResult) {
            std::cout << "✓ My calculation matches built-in perft!\n";
            std::cout << "The issue is that the built-in perft gives " << builtinResult 
                      << " but expected is 2218, difference of " << ((int64_t)builtinResult - 2218) << "\n";
        } else {
            std::cout << "✗ My calculation doesn't match built-in perft\n";
        }
        
        // Show some legal black moves and their white response counts
        std::cout << "\nTop 10 legal black moves and their white response counts:\n";
        std::sort(moveAnalysis.begin(), moveAnalysis.end(), 
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        for (size_t i = 0; i < std::min(size_t(10), moveAnalysis.size()); i++) {
            const auto& [move, count] = moveAnalysis[i];
            std::cout << move << ": " << count << " white responses\n";
        }
    }
};

int main() {
    CorrectedPerftAnalysis::analyzeE2D1Perft();
    return 0;
}
