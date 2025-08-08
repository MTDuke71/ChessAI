#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== CHECKING LEGAL WHITE PAWN MOVES ===\n";
    
    std::vector<std::string> potentialMoves = {"a2-a3", "a2-a4", "b2-b3", "b2-b4", "c2-c3", "c2-c4", "f2-f3", "f2-f4", "g2-g3", "g2-g4", "h2-h3", "h2-h4"};
    
    for (const std::string& move : potentialMoves) {
        bool legal = board.isMoveLegal(move);
        std::cout << move << ": " << (legal ? "LEGAL" : "ILLEGAL") << "\n";
        
        if (legal && move == "g2-g4") {
            Board copy = board;
            Board::MoveState state;
            copy.makeMove(move, state);
            
            std::cout << "  After " << move << " - en passant square: " << copy.getEnPassantSquare() << "\n";
            
            // Check for en passant responses
            MoveGenerator gen;
            auto moves = gen.generateAllMoves(copy, false);
            
            int enPassantCount = 0;
            for (auto blackMove : moves) {
                if (moveSpecial(blackMove) == 2) {
                    enPassantCount++;
                    std::cout << "    En passant move: " << decodeMove(blackMove) << "\n";
                }
            }
            
            if (enPassantCount == 0) {
                std::cout << "    No en passant moves available\n";
            }
        }
    }
    
    return 0;
}
