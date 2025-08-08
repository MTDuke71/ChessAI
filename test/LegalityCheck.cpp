#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== COMPARING GENERATED VS LEGAL MOVES ===\n";
    
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(board, true);
    
    std::cout << "Total generated moves: " << moves.size() << "\n";
    
    int legalCount = 0;
    int illegalCount = 0;
    
    std::vector<std::string> illegalMoves;
    
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        bool legal = board.isMoveLegal(moveStr);
        
        if (legal) {
            legalCount++;
        } else {
            illegalCount++;
            illegalMoves.push_back(moveStr);
        }
    }
    
    std::cout << "Legal moves: " << legalCount << "\n";
    std::cout << "Illegal moves: " << illegalCount << "\n";
    
    if (illegalCount > 0) {
        std::cout << "\nIllegal moves generated:\n";
        for (const std::string& move : illegalMoves) {
            std::cout << "  " << move << "\n";
        }
    }
    
    // This should match our depth 1 perft count
    std::cout << "\nExpected depth 1 count: 48\n";
    std::cout << "Actual legal move count: " << legalCount << "\n";
    
    return 0;
}
