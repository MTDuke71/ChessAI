#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <map>

int main() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    
    Board board;
    board.loadFEN(fen);
    MoveGenerator gen;
    
    std::cout << "=== MOVE-BY-MOVE PERFT ANALYSIS ===\n";
    std::cout << "FEN: " << fen << "\n\n";
    
    // Expected values from ChessProgramming.org (Position 2)
    std::map<std::string, uint64_t> expectedValues = {
        {"a2-a3", 2182}, {"b2-b3", 1970}, {"g2-g3", 1961}, {"d5-d6", 1958}, 
        {"a2-a4", 2196}, {"g2-g4", 1957}, {"g2-h3", 2021}, {"d5-e6", 2237}, 
        {"c3-b1", 1970}, {"c3-d1", 1970}, {"c3-a4", 2199}, {"c3-b5", 2134}, 
        {"e5-d3", 2021}, {"e5-c4", 1957}, {"e5-g4", 2021}, {"e5-c6", 1958}, 
        {"e5-g6", 1961}, {"e5-d7", 2122}, {"e5-f7", 2078}, {"d2-c1", 2001}, 
        {"d2-e3", 2129}, {"d2-f4", 2001}, {"d2-g5", 2127}, {"d2-h6", 1958}, 
        {"e2-d1", 2218}, {"e2-f1", 2057}, {"e2-d3", 1970}, {"e2-c4", 2076}, 
        {"e2-b5", 2051}, {"e2-a6", 1538}, {"a1-b1", 2001}, {"a1-c1", 2001}, 
        {"a1-d1", 2001}, {"h1-f1", 2001}, {"h1-g1", 2001}, {"f3-d3", 1970}, 
        {"f3-e3", 2170}, {"f3-g3", 2210}, {"f3-h3", 2356}, {"f3-f4", 2128}, 
        {"f3-g4", 2165}, {"f3-f5", 2392}, {"f3-h5", 2263}, {"f3-f6", 2107}, 
        {"e1-d1", 2001}, {"e1-f1", 2001}, {"O-O", 2001}, {"O-O-O", 2001}
    };
    
    std::cout << "Running perft divide and comparing with expected values:\n";
    std::cout << "Format: move: actual (expected) [difference]\n\n";
    
    uint64_t totalActual = 0;
    uint64_t totalExpected = 0;
    int matches = 0;
    int mismatches = 0;
    
    auto moves = gen.generateAllMoves(board, true);
    for (auto m : moves) {
        std::string moveStr = decodeMove(m);
        Board::MoveState st;
        board.makeMove(moveStr, st);
        uint64_t actualNodes = 0;
        if (!gen.isKingInCheck(board, !board.isWhiteToMove())) {
            actualNodes = perft(board, gen, 2);
            totalActual += actualNodes;
        }
        board.unmakeMove(st);
        
        auto expectedIt = expectedValues.find(moveStr);
        if (expectedIt != expectedValues.end()) {
            uint64_t expectedNodes = expectedIt->second;
            totalExpected += expectedNodes;
            
            int64_t difference = (int64_t)actualNodes - (int64_t)expectedNodes;
            
            if (difference == 0) {
                matches++;
            } else {
                mismatches++;
                std::cout << moveStr << ": " << actualNodes << " (" << expectedNodes << ") [" << difference << "]\n";
            }
        } else {
            std::cout << "UNKNOWN MOVE: " << moveStr << " = " << actualNodes << "\n";
        }
    }
    
    std::cout << "\n=== SUMMARY ===\n";
    std::cout << "Total actual: " << totalActual << "\n";
    std::cout << "Total expected: " << totalExpected << "\n";
    std::cout << "Overall difference: " << (int64_t)totalActual - (int64_t)totalExpected << "\n";
    std::cout << "Exact matches: " << matches << "\n";
    std::cout << "Mismatches: " << mismatches << "\n";
    std::cout << "Accuracy: " << (100.0 * totalActual / totalExpected) << "%\n";
    
    return 0;
}
