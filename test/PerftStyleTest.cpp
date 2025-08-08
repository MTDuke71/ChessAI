#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== PERFT-STYLE VERIFICATION ===\n";
    
    MoveGenerator gen;
    auto whiteMoves = gen.generateAllMoves(board, true);
    
    std::cout << "White moves: " << whiteMoves.size() << "\n";
    
    int totalLegalBlackMoves = 0;
    int totalGeneratedBlackMoves = 0;
    
    for (auto whiteMove : whiteMoves) {
        Board copy = board;
        Board::MoveState state;
        std::string moveStr = decodeMove(whiteMove);
        copy.makeMove(moveStr, state);
        
        // Check if this white move is legal (doesn't leave white king in check)
        if (gen.isKingInCheck(copy, true)) {
            std::cout << "Illegal white move: " << moveStr << " (leaves white king in check)\n";
            continue; // Skip this illegal white move
        }
        
        auto blackMoves = gen.generateAllMoves(copy, false);
        int legalBlackMoves = 0;
        
        for (auto blackMove : blackMoves) {
            Board copy2 = copy;
            Board::MoveState state2;
            std::string blackMoveStr = decodeMove(blackMove);
            copy2.makeMove(blackMoveStr, state2);
            
            // Check if this black move is legal (doesn't leave black king in check)
            if (!gen.isKingInCheck(copy2, false)) {
                legalBlackMoves++;
            }
        }
        
        totalGeneratedBlackMoves += blackMoves.size();
        totalLegalBlackMoves += legalBlackMoves;
        
        if (blackMoves.size() != legalBlackMoves) {
            std::cout << moveStr << ": " << blackMoves.size() << " generated, " << legalBlackMoves << " legal ("
                      << (blackMoves.size() - legalBlackMoves) << " illegal)\n";
        }
    }
    
    std::cout << "Total generated black moves: " << totalGeneratedBlackMoves << "\n";
    std::cout << "Total legal black moves: " << totalLegalBlackMoves << "\n";
    std::cout << "Expected: 2039\n";
    std::cout << "Legal difference: " << (totalLegalBlackMoves - 2039) << "\n";
    
    return 0;
}
