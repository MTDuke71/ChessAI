#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <map>
#include <iomanip>

int main() {
    std::cout << "=== PERFT DIVIDE DEPTH 3 COMPARISON ===\n";
    
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    // Expected values from reference
    std::map<std::string, int> expected = {
        {"a1-b1", 1969}, {"a1-c1", 1968}, {"a1-d1", 1885}, {"a2-a3", 2186}, {"a2-a4", 2149},
        {"b2-b3", 1964}, {"c3-a4", 2203}, {"c3-b1", 2038}, {"c3-b5", 2138}, {"c3-d1", 2040},
        {"d2-c1", 1963}, {"d2-e3", 2136}, {"d2-f4", 2000}, {"d2-g5", 2134}, {"d2-h6", 2019},
        {"d5-d6", 1991}, {"d5-e6", 2241}, {"e1-c1", 1887}, {"e1-d1", 1894}, {"e1-f1", 1855},
        {"e1-g1", 2059}, {"e2-a6", 1907}, {"e2-b5", 2057}, {"e2-c4", 2082}, {"e2-d1", 1733},
        {"e2-d3", 2050}, {"e2-f1", 2060}, {"e5-c4", 1880}, {"e5-c6", 2027}, {"e5-d3", 1803},
        {"e5-d7", 2124}, {"e5-f7", 2080}, {"e5-g4", 1878}, {"e5-g6", 1997}, {"f3-d3", 2005},
        {"f3-e3", 2174}, {"f3-f4", 2132}, {"f3-f5", 2396}, {"f3-f6", 2111}, {"f3-g3", 2214},
        {"f3-g4", 2169}, {"f3-h3", 2360}, {"f3-h5", 2267}, {"g2-g3", 1882}, {"g2-g4", 1843},
        {"g2-h3", 1970}, {"h1-f1", 1929}, {"h1-g1", 2013}
    };
    
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(board, true);
    
    std::cout << "Move Comparison (Actual vs Expected):\n";
    std::cout << "Move      | Actual | Expected | Diff\n";
    std::cout << "----------|--------|----------|-----\n";
    
    int totalActual = 0;
    int totalExpected = 0;
    int significantDiffs = 0;
    
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        if (!board.isMoveLegal(moveStr)) continue;
        
        Board copy = board;
        Board::MoveState state;
        copy.makeMove(moveStr, state);
        
        uint64_t actual = perft(copy, gen, 2);
        totalActual += actual;
        
        // Convert castling notation
        std::string lookupMove = moveStr;
        if (moveStr == "O-O") lookupMove = "e1-g1";
        else if (moveStr == "O-O-O") lookupMove = "e1-c1";
        
        if (expected.find(lookupMove) != expected.end()) {
            int exp = expected[lookupMove];
            int diff = (int)actual - exp;
            totalExpected += exp;
            
            if (abs(diff) > 2) { // Only show significant differences
                significantDiffs++;
                std::cout << std::left << std::setw(10) << moveStr 
                         << "| " << std::setw(6) << actual 
                         << " | " << std::setw(8) << exp 
                         << " | " << std::setw(4) << diff << "\n";
            }
        } else {
            std::cout << std::left << std::setw(10) << moveStr 
                     << "| " << std::setw(6) << actual 
                     << " | MISSING  | ?\n";
        }
    }
    
    std::cout << "\n=== SUMMARY ===\n";
    std::cout << "Total Actual:   " << totalActual << "\n";
    std::cout << "Total Expected: " << totalExpected << "\n";
    std::cout << "Total Diff:     " << (totalActual - totalExpected) << "\n";
    std::cout << "Moves with significant differences (>2): " << significantDiffs << "\n";
    
    return 0;
}
