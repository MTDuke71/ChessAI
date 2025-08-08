#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>

class DetailedE2D1Debug {
public:
    static void analyzeE2D1Move() {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        
        Board board;
        board.loadFEN(fen);
        
        std::cout << "=== DETAILED E2-D1 MOVE ANALYSIS ===\n";
        
        // Make the e2-d1 move
        Board::MoveState state;
        board.makeMove("e2-d1", state);
        
        std::cout << "Position after e2-d1:\n";
        board.printBoard();
        std::cout << "\n";
        
        // Generate black's moves
        MoveGenerator gen;
        auto blackMoves = gen.generateAllMoves(board, false);
        
        std::cout << "Generated " << blackMoves.size() << " black moves\n";
        
        // Test the first few black moves in detail
        int moveCount = 0;
        for (auto blackMove : blackMoves) {
            if (moveCount >= 5) break; // Just test first 5 moves
            
            std::string blackMoveStr = decodeMove(blackMove);
            std::cout << "\n--- Testing black move: " << blackMoveStr << " ---\n";
            
            // Check if the move is legal
            bool isLegal = board.isMoveLegal(blackMoveStr);
            std::cout << "Is legal: " << isLegal << "\n";
            
            if (!isLegal) {
                moveCount++;
                continue;
            }
            
            // Make the black move
            Board copy = board;
            Board::MoveState blackState;
            copy.makeMove(blackMoveStr, blackState);
            std::cout << "Move made\n";
            
            std::cout << "Position after " << blackMoveStr << ":\n";
            copy.printBoard();
            
            // Generate white's response moves
            auto whiteMoves = gen.generateAllMoves(copy, true);
            std::cout << "Generated " << whiteMoves.size() << " white moves\n";
            
            // Count legal white moves
            int legalWhiteMoves = 0;
            for (auto whiteMove : whiteMoves) {
                std::string whiteMoveStr = decodeMove(whiteMove);
                if (copy.isMoveLegal(whiteMoveStr)) {
                    legalWhiteMoves++;
                    if (legalWhiteMoves <= 3) { // Show first 3 legal moves
                        std::cout << "  Legal white move: " << whiteMoveStr << "\n";
                    }
                }
            }
            
            std::cout << "Total legal white moves: " << legalWhiteMoves << "\n";
            
            moveCount++;
        }
        
        // Let's also check if the position after e2-d1 is valid
        std::cout << "\n=== POSITION VALIDATION ===\n";
        // Position validation without isInCheck method
        
        // Test a simple move generation on the original position
        std::cout << "\n=== CONTROL TEST ===\n";
        Board originalBoard;
        originalBoard.loadFEN(fen);
        auto originalMoves = gen.generateAllMoves(originalBoard, true);
        std::cout << "Original position generates " << originalMoves.size() << " white moves\n";
    }

private:
    static std::string decodeMove(uint16_t move) {
        int from = (move >> 10) & 0x3F;
        int to = (move >> 4) & 0x3F;
        int flags = move & 0xF;
        
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
        
        // Handle special moves
        if (flags == 0x2) result += " (en passant)";
        else if (flags == 0x3) result += " (castling)";
        else if (flags >= 0x8) {
            result += " (promotion to ";
            switch (flags & 0x7) {
                case 0x0: result += "queen)"; break;
                case 0x1: result += "rook)"; break;
                case 0x2: result += "bishop)"; break;
                case 0x3: result += "knight)"; break;
            }
        }
        
        return result;
    }
};

int main() {
    DetailedE2D1Debug::analyzeE2D1Move();
    return 0;
}
