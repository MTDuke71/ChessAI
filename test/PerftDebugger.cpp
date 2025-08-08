#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <algorithm>

class PerftDebugger {
public:
    static void debugPosition() {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        
        Board board;
        board.loadFEN(fen);
        
        std::cout << "=== DEBUGGING PERFT POSITION ===\n";
        std::cout << "FEN: " << fen << "\n";
        std::cout << "Expected D1: 48, D2: 2039\n\n";
        
        // Test depth 1 first
        debugDepth1(board);
        
        // If depth 1 is correct, test depth 2
        std::cout << "\n=== DEPTH 2 ANALYSIS ===\n";
        debugDepth2(board);
        
        // Now test depth 3 to find the 5 missing moves
        std::cout << "\n=== DEPTH 3 ANALYSIS ===\n";
        debugDepth3(board);
    }
    
private:
    static void debugDepth1(const Board& board) {
        std::cout << "=== DEPTH 1 ANALYSIS ===\n";
        
        MoveGenerator gen;
        auto moves = gen.generateAllMoves(board, true); // White to move
        
        std::cout << "Generated moves count: " << moves.size() << "\n";
        std::cout << "Expected: 48\n\n";
        
        // Count by piece type
        int pawnMoves = 0, knightMoves = 0, bishopMoves = 0;
        int rookMoves = 0, queenMoves = 0, kingMoves = 0;
        int castlingMoves = 0, enPassantMoves = 0;
        
        std::vector<std::string> allMoves;
        
        for (auto move : moves) {
            std::string moveStr = decodeMove(move);
            allMoves.push_back(moveStr);
            
            if (!board.isMoveLegal(moveStr)) {
                std::cout << "ILLEGAL MOVE GENERATED: " << moveStr << "\n";
                continue;
            }
            
            // Determine piece type by checking bitboards
            int from = moveFrom(move);
            uint64_t fromMask = 1ULL << from;
            
            if (board.getWhitePawns() & fromMask) pawnMoves++;
            else if (board.getWhiteKnights() & fromMask) knightMoves++;
            else if (board.getWhiteBishops() & fromMask) bishopMoves++;
            else if (board.getWhiteRooks() & fromMask) rookMoves++;
            else if (board.getWhiteQueens() & fromMask) queenMoves++;
            else if (board.getWhiteKing() & fromMask) {
                kingMoves++;
                if (moveSpecial(move) == 3) castlingMoves++;
            }
            
            if (moveSpecial(move) == 2) enPassantMoves++;
        }
        
        std::cout << "Move breakdown:\n";
        std::cout << "  Pawn moves: " << pawnMoves << "\n";
        std::cout << "  Knight moves: " << knightMoves << "\n";
        std::cout << "  Bishop moves: " << bishopMoves << "\n";
        std::cout << "  Rook moves: " << rookMoves << "\n";
        std::cout << "  Queen moves: " << queenMoves << "\n";
        std::cout << "  King moves: " << kingMoves << "\n";
        std::cout << "  Castling moves: " << castlingMoves << "\n";
        std::cout << "  En passant moves: " << enPassantMoves << "\n";
        
        // Show all moves for manual verification
        std::cout << "\nAll generated moves:\n";
        std::sort(allMoves.begin(), allMoves.end());
        for (size_t i = 0; i < allMoves.size(); ++i) {
            std::cout << allMoves[i];
            if ((i + 1) % 8 == 0) std::cout << "\n";
            else std::cout << " ";
        }
        std::cout << "\n";
    }
    
    static void debugDepth2(const Board& board) {
        MoveGenerator gen;
        auto moves = gen.generateAllMoves(board, true);
        
        uint64_t totalNodes = 0;
        std::vector<std::pair<std::string, uint64_t>> moveResults;
        
        for (auto move : moves) {
            std::string moveStr = decodeMove(move);
            if (!board.isMoveLegal(moveStr)) continue;
            
            Board copy = board;
            Board::MoveState state;
            copy.makeMove(moveStr, state);
            
            auto responseMoves = gen.generateAllMoves(copy, false); // Black's response
            uint64_t legalResponses = 0;
            
            for (auto responseMove : responseMoves) {
                std::string responseStr = decodeMove(responseMove);
                if (copy.isMoveLegal(responseStr)) {
                    legalResponses++;
                }
            }
            
            totalNodes += legalResponses;
            moveResults.push_back({moveStr, legalResponses});
        }
        
        std::cout << "Total D2 nodes: " << totalNodes << "\n";
        std::cout << "Expected: 2039\n";
        std::cout << "Difference: " << (int64_t)totalNodes - 2039 << "\n\n";
        
        // Show moves with unusual response counts
        std::cout << "Moves with their response counts:\n";
        std::sort(moveResults.begin(), moveResults.end(), 
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        for (const auto& [move, count] : moveResults) {
            std::cout << move << ": " << count << "\n";
        }
    }
    
    static void debugDepth3(const Board& board) {
        MoveGenerator gen;
        auto moves = gen.generateAllMoves(board, true);
        
        uint64_t totalNodes = 0;
        std::vector<std::pair<std::string, uint64_t>> moveResults;
        
        for (auto move : moves) {
            std::string moveStr = decodeMove(move);
            if (!board.isMoveLegal(moveStr)) continue;
            
            Board copy = board;
            Board::MoveState state;
            copy.makeMove(moveStr, state);
            
            // Get all legal moves for black
            auto responseMoves = gen.generateAllMoves(copy, false);
            uint64_t moveTotal = 0;
            
            for (auto responseMove : responseMoves) {
                std::string responseStr = decodeMove(responseMove);
                if (!copy.isMoveLegal(responseStr)) continue;
                
                // Make black's move and count white's responses
                Board copy2 = copy;
                Board::MoveState state2;
                copy2.makeMove(responseStr, state2);
                
                auto finalMoves = gen.generateAllMoves(copy2, true);
                uint64_t legalFinalMoves = 0;
                
                for (auto finalMove : finalMoves) {
                    std::string finalStr = decodeMove(finalMove);
                    if (copy2.isMoveLegal(finalStr)) {
                        legalFinalMoves++;
                    }
                }
                
                moveTotal += legalFinalMoves;
            }
            
            totalNodes += moveTotal;
            moveResults.push_back({moveStr, moveTotal});
        }
        
        std::cout << "Total D3 nodes: " << totalNodes << "\n";
        std::cout << "Expected: 97862\n";
        std::cout << "Difference: " << (int64_t)totalNodes - 97862 << "\n\n";
        
        // Show moves with unusual counts (sorted by difference from expected)
        std::cout << "Top moves by D3 count:\n";
        std::sort(moveResults.begin(), moveResults.end(), 
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        for (size_t i = 0; i < std::min(20ULL, moveResults.size()); ++i) {
            const auto& [move, count] = moveResults[i];
            std::cout << move << ": " << count << "\n";
        }
    }
};

int main() {
    PerftDebugger::debugPosition();
    return 0;
}
