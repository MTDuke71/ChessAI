#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>
#include <set>
#include <string>

// Known correct moves for this position at depth 1 (from reference engine)
std::vector<std::string> referenceMovesD1 = {
    "a1-b1", "a1-c1", "a1-d1", "a2-a3", "a2-a4", "b2-b3", "c3-a4", "c3-b1",
    "c3-b5", "c3-d1", "d2-c1", "d2-e3", "d2-f4", "d2-g5", "d2-h6", "d5-d6",
    "d5-e6", "e1-f1", "e1-g1", "e2-a6", "e2-b5", "e2-c4", "e2-d1", "e2-d3",
    "e2-f1", "e5-c4", "e5-c6", "e5-d3", "e5-d7", "e5-f7", "e5-g4", "e5-g6",
    "f3-d3", "f3-e3", "f3-f4", "f3-f5", "f3-f6", "f3-g3", "f3-g4", "f3-h3",
    "f3-h5", "g2-g3", "g2-g4", "g2-h3", "h1-f1", "h1-g1", "O-O", "O-O-O"
};

void compareMoves() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    
    Board board;
    board.loadFEN(fen);
    
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(board, true);
    
    std::set<std::string> generatedMoves;
    std::set<std::string> referenceMoves(referenceMovesD1.begin(), referenceMovesD1.end());
    
    std::cout << "=== MOVE COMPARISON ===\n";
    std::cout << "Generated: " << moves.size() << " moves\n";
    std::cout << "Reference: " << referenceMoves.size() << " moves\n\n";
    
    // Convert generated moves to strings
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        if (board.isMoveLegal(moveStr)) {
            generatedMoves.insert(moveStr);
        }
    }
    
    // Find missing moves
    std::vector<std::string> missing;
    for (const auto& refMove : referenceMoves) {
        if (generatedMoves.find(refMove) == generatedMoves.end()) {
            missing.push_back(refMove);
        }
    }
    
    // Find extra moves
    std::vector<std::string> extra;
    for (const auto& genMove : generatedMoves) {
        if (referenceMoves.find(genMove) == referenceMoves.end()) {
            extra.push_back(genMove);
        }
    }
    
    if (!missing.empty()) {
        std::cout << "MISSING MOVES (" << missing.size() << "):\n";
        for (const auto& move : missing) {
            std::cout << "  " << move << "\n";
        }
    }
    
    if (!extra.empty()) {
        std::cout << "EXTRA MOVES (" << extra.size() << "):\n";
        for (const auto& move : extra) {
            std::cout << "  " << move << "\n";
        }
    }
    
    if (missing.empty() && extra.empty()) {
        std::cout << "✅ All moves match perfectly!\n";
    }
}

int main() {
    compareMoves();
    return 0;
}
