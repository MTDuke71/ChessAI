#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <map>

class MoveAnalyzer {
public:
    static void analyzeSpecificMoves() {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        Board board;
        board.loadFEN(fen);
        
        std::cout << "=== DETAILED DEPTH 2 MOVE ANALYSIS ===\n";
        
        // Let's compare the expected vs actual response counts for key moves
        std::map<std::string, int> expectedCounts = {
            {"d5-e6", 46},  // Should have highest response count
            {"e5-d7", 45},
            {"f3-f5", 45},
            {"a2-a3", 44},
            {"e2-d1", 44},
            {"O-O", 43},    // Castling moves
            {"O-O-O", 43}
        };
        
        MoveGenerator gen;
        auto moves = gen.generateAllMoves(board, true);
        
        for (auto move : moves) {
            std::string moveStr = decodeMove(move);
            if (!board.isMoveLegal(moveStr)) continue;
            
            // Focus on moves that should have specific response counts
            if (expectedCounts.find(moveStr) != expectedCounts.end()) {
                analyzeMove(board, moveStr, expectedCounts[moveStr]);
            }
        }
        
        // Also analyze the castling situation in detail
        std::cout << "\n=== CASTLING ANALYSIS ===\n";
        analyzeCastlingMoves(board);
    }

private:
    static void analyzeMove(const Board& board, const std::string& moveStr, int expected) {
        std::cout << "\n--- Analyzing " << moveStr << " ---\n";
        std::cout << "Expected responses: " << expected << "\n";
        
        Board copy = board;
        Board::MoveState state;
        copy.makeMove(moveStr, state);
        
        MoveGenerator gen;
        auto responseMoves = gen.generateAllMoves(copy, false);
        
        int legalResponses = 0;
        int illegalResponses = 0;
        std::vector<std::string> illegalMovesList;
        
        for (auto responseMove : responseMoves) {
            std::string responseStr = decodeMove(responseMove);
            if (copy.isMoveLegal(responseStr)) {
                legalResponses++;
            } else {
                illegalResponses++;
                illegalMovesList.push_back(responseStr);
            }
        }
        
        std::cout << "Generated responses: " << responseMoves.size() << "\n";
        std::cout << "Legal responses: " << legalResponses << "\n";
        std::cout << "Illegal responses: " << illegalResponses << "\n";
        std::cout << "Difference from expected: " << (legalResponses - expected) << "\n";
        
        if (illegalResponses > 0) {
            std::cout << "Illegal moves generated:\n";
            for (const auto& illegalMove : illegalMovesList) {
                std::cout << "  " << illegalMove << "\n";
            }
        }
        
        // Check if the board state looks correct after the move
        std::cout << "Board state after " << moveStr << ":\n";
        std::cout << "  White to move: " << (copy.isWhiteToMove() ? "NO" : "YES") << " (should be NO)\n";
        std::cout << "  En passant square: " << copy.getEnPassantSquare() << "\n";
        std::cout << "  Can castle WK: " << copy.canCastleWK() << "\n";
        std::cout << "  Can castle WQ: " << copy.canCastleWQ() << "\n";
        std::cout << "  Can castle BK: " << copy.canCastleBK() << "\n";
        std::cout << "  Can castle BQ: " << copy.canCastleBQ() << "\n";
    }
    
    static void analyzeCastlingMoves(const Board& board) {
        std::cout << "Initial castling rights:\n";
        std::cout << "  White kingside: " << board.canCastleWK() << "\n";
        std::cout << "  White queenside: " << board.canCastleWQ() << "\n";
        std::cout << "  Black kingside: " << board.canCastleBK() << "\n";
        std::cout << "  Black queenside: " << board.canCastleBQ() << "\n";
        
        // Test O-O specifically
        std::cout << "\nTesting O-O move:\n";
        if (board.isMoveLegal("O-O")) {
            Board copy = board;
            Board::MoveState state;
            copy.makeMove("O-O", state);
            
            std::cout << "  After O-O - White castling rights should be lost:\n";
            std::cout << "    Can castle WK: " << copy.canCastleWK() << " (should be 0)\n";
            std::cout << "    Can castle WQ: " << copy.canCastleWQ() << " (should be 0)\n";
            
            // Check piece positions
            std::cout << "  King should be on g1, Rook on f1\n";
            uint64_t kingOnG1 = copy.getWhiteKing() & (1ULL << 6);
            uint64_t rookOnF1 = copy.getWhiteRooks() & (1ULL << 5);
            std::cout << "    King on g1: " << (kingOnG1 ? "YES" : "NO") << "\n";
            std::cout << "    Rook on f1: " << (rookOnF1 ? "YES" : "NO") << "\n";
        }
        
        // Test O-O-O specifically  
        std::cout << "\nTesting O-O-O move:\n";
        if (board.isMoveLegal("O-O-O")) {
            Board copy = board;
            Board::MoveState state;
            copy.makeMove("O-O-O", state);
            
            std::cout << "  After O-O-O - White castling rights should be lost:\n";
            std::cout << "    Can castle WK: " << copy.canCastleWK() << " (should be 0)\n";
            std::cout << "    Can castle WQ: " << copy.canCastleWQ() << " (should be 0)\n";
            
            // Check piece positions
            std::cout << "  King should be on c1, Rook on d1\n";
            uint64_t kingOnC1 = copy.getWhiteKing() & (1ULL << 2);
            uint64_t rookOnD1 = copy.getWhiteRooks() & (1ULL << 3);
            std::cout << "    King on c1: " << (kingOnC1 ? "YES" : "NO") << "\n";
            std::cout << "    Rook on d1: " << (rookOnD1 ? "YES" : "NO") << "\n";
        }
    }
};

int main() {
    MoveAnalyzer::analyzeSpecificMoves();
    return 0;
}
