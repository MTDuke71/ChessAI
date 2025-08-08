#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <algorithm>

class FixedE2D1Analysis {
public:
    static void analyzeE2D1Move() {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        
        Board board;
        board.loadFEN(fen);
        
        std::cout << "=== FIXED E2-D1 MOVE ANALYSIS ===\n";
        std::cout << "Expected depth 3 nodes after e2-d1: 2218\n";
        
        // Make the e2-d1 move
        Board::MoveState state;
        board.makeMove("e2-d1", state);
        
        std::cout << "Position after e2-d1:\n";
        board.printBoard();
        std::cout << "\n";
        
        // Now analyze black's responses
        MoveGenerator gen;
        auto blackMoves = gen.generateAllMoves(board, false);
        
        std::cout << "Black's possible responses: " << blackMoves.size() << "\n";
        
        uint64_t totalDepth2Nodes = 0;
        std::vector<std::pair<std::string, uint64_t>> moveAnalysis;
        
        for (auto blackMove : blackMoves) {
            std::string blackMoveStr = decodeMove(blackMove);
            if (!board.isMoveLegal(blackMoveStr)) continue;
            
            Board copy = board;
            Board::MoveState blackState;
            copy.makeMove(blackMoveStr, blackState);
            
            // Count white's responses (depth 2 from e2-d1)
            auto whiteMoves = gen.generateAllMoves(copy, true);
            uint64_t whiteResponses = 0;
            
            for (auto whiteMove : whiteMoves) {
                std::string whiteMoveStr = decodeMove(whiteMove);
                if (copy.isMoveLegal(whiteMoveStr)) {
                    whiteResponses++;
                }
            }
            
            totalDepth2Nodes += whiteResponses;
            moveAnalysis.push_back({blackMoveStr, whiteResponses});
        }
        
        std::cout << "Total depth 2 nodes after e2-d1: " << totalDepth2Nodes << "\n";
        std::cout << "Expected: 2218\n";
        std::cout << "Difference: " << (int64_t)totalDepth2Nodes - 2218 << "\n\n";
        
        // Show each black move and its white response count
        std::cout << "Black moves after e2-d1 and their white response counts:\n";
        std::sort(moveAnalysis.begin(), moveAnalysis.end(), 
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        for (const auto& [move, count] : moveAnalysis) {
            std::cout << move << ": " << count << " white responses\n";
        }
        
        // Compare with a few expected values we can validate
        std::cout << "\n=== EXPECTED VS ACTUAL ANALYSIS ===\n";
        
        // Let's manually test a few key moves that should generate many responses
        testSpecificMove(board, "b4-b3", gen);
        testSpecificMove(board, "h3-g2", gen);
        testSpecificMove(board, "O-O", gen);
        testSpecificMove(board, "e7-f6", gen);
    }

private:
    static void testSpecificMove(const Board& board, const std::string& moveStr, MoveGenerator& gen) {
        if (!board.isMoveLegal(moveStr)) {
            std::cout << "Move " << moveStr << " is not legal\n";
            return;
        }
        
        Board copy = board;
        Board::MoveState state;
        copy.makeMove(moveStr, state);
        
        auto whiteMoves = gen.generateAllMoves(copy, true);
        uint64_t whiteResponses = 0;
        
        for (auto whiteMove : whiteMoves) {
            std::string whiteMoveStr = decodeMove(whiteMove);
            if (copy.isMoveLegal(whiteMoveStr)) {
                whiteResponses++;
            }
        }
        
        std::cout << "After " << moveStr << ": " << whiteResponses << " white responses\n";
    }
    
    static std::string decodeMove(uint16_t move) {
        int to = move & 0x3f;
        int from = (move >> 6) & 0x3f;
        int promo = (move >> 12) & 0x3;
        int special = (move >> 14) & 0x3;
        
        // Handle castling moves
        if (special == 3) {
            // Check if it's kingside or queenside castling
            if (to - from == 2) {
                return "O-O";   // Kingside: king moves 2 squares right
            } else if (from - to == 2) {
                return "O-O-O"; // Queenside: king moves 2 squares left
            }
        }
        
        char fromFile = 'a' + (from % 8);
        char fromRank = '1' + (from / 8);
        char toFile = 'a' + (to % 8);
        char toRank = '1' + (to / 8);
        
        std::string result;
        result += fromFile;
        result += fromRank;
        result += '-';
        result += toFile;
        result += toRank;
        
        // Handle promotion
        if (special == 1) {
            switch (promo) {
                case 0: result += "n"; break;
                case 1: result += "b"; break;
                case 2: result += "r"; break;
                case 3: result += "q"; break;
            }
        }
        
        return result;
    }
};

int main() {
    FixedE2D1Analysis::analyzeE2D1Move();
    return 0;
}
