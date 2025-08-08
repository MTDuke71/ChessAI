#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

int main() {
    std::cout << "=== PERFT DIVIDE DEPTH 2 ===\n";
    
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    std::cout << "FEN: " << board.getFEN() << "\n";
    std::cout << "Expected total for depth 2: 2039\n\n";
    
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
        
        // Use built-in perft for consistency
        uint64_t nodeCount = perft(copy, gen, 1);
        
        totalNodes += nodeCount;
        moveResults.push_back({moveStr, nodeCount});
    }
    
    // Convert castling moves to standard notation and sort in reference order
    std::vector<std::pair<std::string, uint64_t>> sortedResults;
    
    // Define the reference order
    std::vector<std::string> referenceOrder = {
        "a1-b1", "a1-c1", "a1-d1", "a2-a3", "a2-a4", "b2-b3",
        "c3-a4", "c3-b1", "c3-b5", "c3-d1", "d2-c1", "d2-e3", "d2-f4", "d2-g5", "d2-h6",
        "d5-d6", "d5-e6", "e1-c1", "e1-d1", "e1-f1", "e1-g1", 
        "e2-a6", "e2-b5", "e2-c4", "e2-d1", "e2-d3", "e2-f1",
        "e5-c4", "e5-c6", "e5-d3", "e5-d7", "e5-f7", "e5-g4", "e5-g6",
        "f3-d3", "f3-e3", "f3-f4", "f3-f5", "f3-f6", "f3-g3", "f3-g4", "f3-h3", "f3-h5",
        "g2-g3", "g2-g4", "g2-h3", "h1-f1", "h1-g1"
    };
    
    // Convert results and map castling
    for (const auto& [move, count] : moveResults) {
        std::string displayMove = move;
        if (move == "O-O") displayMove = "e1-g1";
        else if (move == "O-O-O") displayMove = "e1-c1";
        sortedResults.push_back({displayMove, count});
    }
    
    std::cout << "PERFT DIVIDE DEPTH 2 RESULTS (Reference Order):\n";
    std::cout << "Total: " << totalNodes << "\n";
    
    // Print in reference order
    for (const std::string& refMove : referenceOrder) {
        for (const auto& [move, count] : sortedResults) {
            if (move == refMove) {
                std::cout << refMove << " - " << count << "\n";
                break;
            }
        }
    }
    
    // Print any moves not in reference order
    std::cout << "\nMoves not in reference:\n";
    for (const auto& [move, count] : sortedResults) {
        bool found = false;
        for (const std::string& refMove : referenceOrder) {
            if (move == refMove) {
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << move << " - " << count << "\n";
        }
    }
    
    std::cout << "\nTotal nodes: " << totalNodes << "\n";
    std::cout << "Expected  : 2039\n";
    std::cout << "Difference: " << (int64_t)totalNodes - 2039 << "\n";
    
    return 0;
}
