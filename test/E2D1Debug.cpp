#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>

class E2D1Debugger {
public:
    static void analyzeE2D1Move() {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        
        Board board;
        board.loadFEN(fen);
        
        std::cout << "=== E2-D1 MOVE ANALYSIS ===\n";
        std::cout << "Expected depth 3 nodes after e2-d1: 2218\n";
        std::cout << "Currently getting: 1733\n";
        std::cout << "Missing: 485 nodes\n\n";
        
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
        for (const auto& [move, count] : moveAnalysis) {
            std::cout << move << ": " << count << " white responses\n";
        }
        
        // Let's also check if there are any special moves we might be missing
        std::cout << "\n=== SPECIAL MOVE ANALYSIS ===\n";
        
        // Check for castling opportunities
        std::cout << "Can black castle kingside? " << board.canCastleBK() << "\n";
        std::cout << "Can black castle queenside? " << board.canCastleBQ() << "\n";
        
        // Check for en passant
        std::cout << "En passant square: " << board.getEnPassantSquare() << "\n";
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
    E2D1Debugger::analyzeE2D1Move();
    return 0;
}
