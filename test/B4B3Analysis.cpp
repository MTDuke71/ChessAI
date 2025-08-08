#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

int main() {
    std::cout << "=== ANALYZING b4-b3 POSITION ===\n";
    
    // Position after O-O followed by b4-b3
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/1pN2Q1p/PPPBBPPP/R4RK1 w kq - 0 2");
    
    std::cout << "FEN: " << board.getFEN() << "\n";
    std::cout << "Expected: 49 white moves\n\n";
    
    // Expected moves from reference
    std::set<std::string> expectedMoves = {
        "a1-b1", "a1-c1", "a1-d1", "a1-e1", "a2-a3", "a2-a4", "a2-b3", "c2-b3",
        "c3-a4", "c3-b1", "c3-b5", "c3-d1", "d2-c1", "d2-e1", "d2-e3", "d2-f4", "d2-g5", "d2-h6",
        "d5-d6", "d5-e6", "e2-a6", "e2-b5", "e2-c4", "e2-d1", "e2-d3",
        "e5-c4", "e5-c6", "e5-d3", "e5-d7", "e5-f7", "e5-g4", "e5-g6",
        "f1-b1", "f1-c1", "f1-d1", "f1-e1", "f3-d3", "f3-e3", "f3-f4", "f3-f5", "f3-f6", "f3-g3", "f3-g4", "f3-h3", "f3-h5",
        "g1-h1", "g2-g3", "g2-g4", "g2-h3"
    };
    
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(board, true); // White to move
    
    std::set<std::string> actualMoves;
    std::vector<std::string> legalMoves;
    
    std::cout << "Generated moves:\n";
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        if (board.isMoveLegal(moveStr)) {
            actualMoves.insert(moveStr);
            legalMoves.push_back(moveStr);
        }
    }
    
    std::cout << "Actual count: " << actualMoves.size() << "\n";
    std::cout << "Expected count: " << expectedMoves.size() << "\n";
    std::cout << "Difference: " << (int)actualMoves.size() - (int)expectedMoves.size() << "\n\n";
    
    // Find missing moves
    std::cout << "MISSING MOVES:\n";
    for (const std::string& expected : expectedMoves) {
        if (actualMoves.find(expected) == actualMoves.end()) {
            std::cout << "❌ " << expected << "\n";
        }
    }
    
    // Find extra moves
    std::cout << "\nEXTRA MOVES:\n";
    for (const std::string& actual : actualMoves) {
        if (expectedMoves.find(actual) == expectedMoves.end()) {
            std::cout << "⚠️  " << actual << "\n";
        }
    }
    
    // Show all actual moves sorted
    std::cout << "\nAll actual moves:\n";
    std::sort(legalMoves.begin(), legalMoves.end());
    for (size_t i = 0; i < legalMoves.size(); ++i) {
        std::cout << legalMoves[i];
        if ((i + 1) % 8 == 0) std::cout << "\n";
        else std::cout << " ";
    }
    std::cout << "\n";
    
    return 0;
}
