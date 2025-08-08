#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== SIMPLE PERFT 2 VERIFICATION ===\n";
    
    MoveGenerator gen;
    auto whiteMoves = gen.generateAllMoves(board, true);
    
    std::cout << "White moves: " << whiteMoves.size() << "\n";
    
    int totalBlackMoves = 0;
    
    for (auto whiteMove : whiteMoves) {
        Board copy = board;
        Board::MoveState state;
        copy.makeMove(decodeMove(whiteMove), state);
        
        auto blackMoves = gen.generateAllMoves(copy, false);
        totalBlackMoves += blackMoves.size();
        
        std::cout << decodeMove(whiteMove) << ": " << blackMoves.size() << " responses\n";
    }
    
    std::cout << "Total depth 2 moves: " << totalBlackMoves << "\n";
    std::cout << "Expected: 2039\n";
    std::cout << "Difference: " << (totalBlackMoves - 2039) << "\n";
    
    return 0;
}
