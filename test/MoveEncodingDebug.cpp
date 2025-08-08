#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>

class MoveEncodingDebug {
public:
    static void debugMoveEncoding() {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        
        Board board;
        board.loadFEN(fen);
        
        std::cout << "=== MOVE ENCODING DEBUG ===\n";
        
        // First, let's see what moves are generated for white in the original position
        MoveGenerator gen;
        auto whiteMoves = gen.generateAllMoves(board, true);
        
        std::cout << "Original position - White moves:\n";
        for (int i = 0; i < std::min(10, (int)whiteMoves.size()); i++) {
            uint16_t move = whiteMoves[i];
            std::string moveStr = decodeMove(move);
            bool isLegal = board.isMoveLegal(moveStr);
            std::cout << i << ": " << moveStr << " (legal: " << isLegal << ")\n";
        }
        
        // Make the e2-d1 move
        Board::MoveState state;
        board.makeMove("e2-d1", state);
        
        std::cout << "\nAfter e2-d1 - Black moves:\n";
        auto blackMoves = gen.generateAllMoves(board, false);
        
        for (int i = 0; i < std::min(10, (int)blackMoves.size()); i++) {
            uint16_t move = blackMoves[i];
            std::string moveStr = decodeMove(move);
            bool isLegal = board.isMoveLegal(moveStr);
            
            std::cout << i << ": " << moveStr << " (legal: " << isLegal << ")\n";
            
            // Let's also show the raw move encoding
            int from = (move >> 10) & 0x3F;
            int to = (move >> 4) & 0x3F;
            int flags = move & 0xF;
            std::cout << "    Raw: from=" << from << " to=" << to << " flags=" << flags << "\n";
        }
        
        // Let's test a simple black move manually
        std::cout << "\n=== MANUAL MOVE TEST ===\n";
        
        // Test a simple pawn move like a7-a6
        std::cout << "Testing a7-a6:\n";
        bool a7a6Legal = board.isMoveLegal("a7-a6");
        std::cout << "a7-a6 is legal: " << a7a6Legal << "\n";
        
        // Test a simple piece move like b6-c4
        std::cout << "Testing b6-c4:\n";
        bool b6c4Legal = board.isMoveLegal("b6-c4");
        std::cout << "b6-c4 is legal: " << b6c4Legal << "\n";
        
        // Let's also check what the current position looks like
        std::cout << "\nCurrent position after e2-d1:\n";
        board.printBoard();
        
        // Check the board state
        std::cout << "\nBoard state:\n";
        std::cout << "White to move: " << board.isWhiteToMove() << "\n";
        std::cout << "Full move number: " << board.getFullmoveNumber() << "\n";
        std::cout << "Half move clock: " << board.getHalfmoveClock() << "\n";
    }

private:
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
            result += " (promotion to ";
            switch (promo) {
                case 0: result += "knight)"; break;
                case 1: result += "bishop)"; break;
                case 2: result += "rook)"; break;
                case 3: result += "queen)"; break;
            }
        } else if (special == 2) {
            result += " (en passant)";
        } else if (special == 3) {
            result += " (castling)";
        }
        
        return result;
    }
};

int main() {
    MoveEncodingDebug::debugMoveEncoding();
    return 0;
}
