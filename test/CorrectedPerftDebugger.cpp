#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <algorithm>

class CorrectedPerftDebugger {
public:
    static void debugPosition() {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        
        Board board;
        board.loadFEN(fen);
        
        std::cout << "=== CORRECTED PERFT DEBUGGING ===\n";
        std::cout << "FEN: " << fen << "\n";
        std::cout << "Expected D1: 48, D2: 2039, D3: 97862\n\n";
        
        // Test depth 1 with perft-style logic
        debugDepth1(board);
        
        // Test depth 2 with perft-style logic
        std::cout << "\n=== DEPTH 2 ANALYSIS (PERFT STYLE) ===\n";
        debugDepth2(board);
        
        // Test depth 3 with perft-style logic
        std::cout << "\n=== DEPTH 3 ANALYSIS (PERFT STYLE) ===\n";
        debugDepth3(board);
    }
    
private:
    static void debugDepth1(const Board& board) {
        std::cout << "=== DEPTH 1 ANALYSIS (PERFT STYLE) ===\n";
        
        MoveGenerator gen;
        auto moves = gen.generateAllMoves(board, true); // White to move
        
        std::cout << "Generated moves count: " << moves.size() << "\n";
        
        uint64_t legalCount = 0;
        std::vector<std::string> legalMoves;
        
        for (auto move : moves) {
            std::string moveStr = decodeMove(move);
            
            Board copy = board;
            Board::MoveState state;
            copy.makeMove(moveStr, state);
            
            // Use the same logic as perft: check if the king that just moved is in check
            if (!gen.isKingInCheck(copy, !copy.isWhiteToMove())) {
                legalCount++;
                legalMoves.push_back(moveStr);
            }
            
            copy.unmakeMove(state);
        }
        
        std::cout << "Legal moves count: " << legalCount << "\n";
        std::cout << "Expected: 48\n";
        std::cout << "Difference: " << (int64_t)legalCount - 48 << "\n\n";
        
        if (legalCount != 48) {
            std::cout << "Legal moves:\n";
            std::sort(legalMoves.begin(), legalMoves.end());
            for (size_t i = 0; i < legalMoves.size(); ++i) {
                std::cout << legalMoves[i];
                if ((i + 1) % 8 == 0) std::cout << "\n";
                else std::cout << " ";
            }
            std::cout << "\n";
        }
    }
    
    static void debugDepth2(const Board& board) {
        MoveGenerator gen;
        auto moves = gen.generateAllMoves(board, true);
        
        uint64_t totalNodes = 0;
        std::vector<std::pair<std::string, uint64_t>> moveResults;
        
        for (auto move : moves) {
            std::string moveStr = decodeMove(move);
            
            Board copy = board;
            Board::MoveState state;
            copy.makeMove(moveStr, state);
            
            // Check if this white move is legal
            if (gen.isKingInCheck(copy, !copy.isWhiteToMove())) {
                copy.unmakeMove(state);
                continue; // Illegal move
            }
            
            // Count black's legal responses
            auto responseMoves = gen.generateAllMoves(copy, false);
            uint64_t legalResponses = 0;
            
            for (auto responseMove : responseMoves) {
                std::string responseStr = decodeMove(responseMove);
                
                Board copy2 = copy;
                Board::MoveState state2;
                copy2.makeMove(responseStr, state2);
                
                // Check if this black move is legal
                if (!gen.isKingInCheck(copy2, !copy2.isWhiteToMove())) {
                    legalResponses++;
                }
                
                copy2.unmakeMove(state2);
            }
            
            totalNodes += legalResponses;
            moveResults.push_back({moveStr, legalResponses});
            copy.unmakeMove(state);
        }
        
        std::cout << "Total D2 nodes: " << totalNodes << "\n";
        std::cout << "Expected: 2039\n";
        std::cout << "Difference: " << (int64_t)totalNodes - 2039 << "\n\n";
        
        if (totalNodes != 2039) {
            // Show moves with their response counts
            std::cout << "Moves with their response counts:\n";
            std::sort(moveResults.begin(), moveResults.end(), 
                      [](const auto& a, const auto& b) { return a.second > b.second; });
            
            for (const auto& [move, count] : moveResults) {
                std::cout << move << ": " << count << "\n";
            }
        }
    }
    
    static void debugDepth3(const Board& board) {
        MoveGenerator gen;
        auto moves = gen.generateAllMoves(board, true);
        
        uint64_t totalNodes = 0;
        std::vector<std::pair<std::string, uint64_t>> moveResults;
        
        for (auto move : moves) {
            std::string moveStr = decodeMove(move);
            
            Board copy = board;
            Board::MoveState state;
            copy.makeMove(moveStr, state);
            
            // Check if this white move is legal
            if (gen.isKingInCheck(copy, !copy.isWhiteToMove())) {
                copy.unmakeMove(state);
                continue; // Illegal move
            }
            
            // Count all depth-2 nodes from this position
            uint64_t depth2Nodes = 0;
            auto responseMoves = gen.generateAllMoves(copy, false);
            
            for (auto responseMove : responseMoves) {
                std::string responseStr = decodeMove(responseMove);
                
                Board copy2 = copy;
                Board::MoveState state2;
                copy2.makeMove(responseStr, state2);
                
                // Check if this black move is legal
                if (gen.isKingInCheck(copy2, !copy2.isWhiteToMove())) {
                    copy2.unmakeMove(state2);
                    continue; // Illegal move
                }
                
                // Count white's legal final moves
                auto finalMoves = gen.generateAllMoves(copy2, true);
                uint64_t legalFinalMoves = 0;
                
                for (auto finalMove : finalMoves) {
                    std::string finalStr = decodeMove(finalMove);
                    
                    Board copy3 = copy2;
                    Board::MoveState state3;
                    copy3.makeMove(finalStr, state3);
                    
                    // Check if this white move is legal
                    if (!gen.isKingInCheck(copy3, !copy3.isWhiteToMove())) {
                        legalFinalMoves++;
                    }
                    
                    copy3.unmakeMove(state3);
                }
                
                depth2Nodes += legalFinalMoves;
                copy2.unmakeMove(state2);
            }
            
            totalNodes += depth2Nodes;
            moveResults.push_back({moveStr, depth2Nodes});
            copy.unmakeMove(state);
        }
        
        std::cout << "Total D3 nodes (perft style): " << totalNodes << "\n";
        std::cout << "Expected: 97862\n";
        std::cout << "Difference: " << (int64_t)totalNodes - 97862 << "\n\n";
        
        // Compare with built-in perft
        Board mutableBoard = board;
        double ms;
        uint64_t builtinResult = perft(mutableBoard, gen, 3, ms);
        std::cout << "Built-in perft(3): " << builtinResult << "\n";
        std::cout << "My calculation: " << totalNodes << "\n";
        std::cout << "Match built-in: " << (totalNodes == builtinResult ? "YES" : "NO") << "\n\n";
        
        if (totalNodes != 97862) {
            // Show top moves by count for analysis
            std::cout << "Top moves by D3 count:\n";
            std::sort(moveResults.begin(), moveResults.end(), 
                      [](const auto& a, const auto& b) { return a.second > b.second; });
            
            for (size_t i = 0; i < std::min(20ULL, moveResults.size()); ++i) {
                const auto& [move, count] = moveResults[i];
                std::cout << move << ": " << count << "\n";
            }
        }
    }
};

int main() {
    CorrectedPerftDebugger::debugPosition();
    return 0;
}
