#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include <algorithm>

int main() {
    std::cout << "=== DEPTH 6 PERFT DIVIDE ANALYSIS ===\n";
    
    Board board;
    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    // Expected values from user's data
    std::map<std::string, uint64_t> expected = {
        {"a2-a3", 4463267}, {"a2-a4", 5363555}, {"b1-a3", 4856835}, {"b1-c3", 5708064},
        {"b2-b3", 5310358}, {"b2-b4", 5293555}, {"c2-c3", 5417640}, {"c2-c4", 5866666},
        {"d2-d3", 8073082}, {"d2-d4", 8879566}, {"e2-e3", 9726018}, {"e2-e4", 9771632},
        {"f2-f3", 4404141}, {"f2-f4", 4890429}, {"g1-f3", 5723523}, {"g1-h3", 4877234},
        {"g2-g3", 5346260}, {"g2-g4", 5239875}, {"h2-h3", 4463070}, {"h2-h4", 5385554}
    };
    
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(board, true);
    
    std::cout << "Move Analysis (Actual vs Expected at Depth 6):\n";
    std::cout << "Move      | Actual    | Expected  | Diff      | %Diff\n";
    std::cout << "----------|-----------|-----------|-----------|------\n";
    
    uint64_t totalActual = 0;
    uint64_t totalExpected = 0;
    int significantDiffs = 0;
    std::vector<std::pair<std::string, int64_t>> differences;
    
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        if (!board.isMoveLegal(moveStr)) continue;
        
        std::cout << "Computing " << moveStr << "..." << std::flush;
        
        Board copy = board;
        Board::MoveState state;
        copy.makeMove(moveStr, state);
        
        uint64_t actual = perft(copy, gen, 5); // Depth 5 from this position = total depth 6
        totalActual += actual;
        
        if (expected.find(moveStr) != expected.end()) {
            uint64_t exp = expected[moveStr];
            int64_t diff = (int64_t)actual - (int64_t)exp;
            totalExpected += exp;
            
            double percentDiff = exp > 0 ? (100.0 * diff / exp) : 0.0;
            
            std::cout << "\r" << std::left << std::setw(10) << moveStr 
                     << "| " << std::setw(9) << actual 
                     << " | " << std::setw(9) << exp 
                     << " | " << std::setw(9) << diff
                     << " | " << std::fixed << std::setprecision(3) << std::setw(5) << percentDiff << "%\n";
            
            if (abs(diff) > 0) {
                significantDiffs++;
                differences.push_back({moveStr, diff});
            }
        } else {
            std::cout << "\r" << std::left << std::setw(10) << moveStr 
                     << "| " << std::setw(9) << actual 
                     << " | MISSING   | ?         | ?\n";
            significantDiffs++;
        }
    }
    
    std::cout << "\n=== SUMMARY ===\n";
    std::cout << "Total Actual:   " << totalActual << "\n";
    std::cout << "Total Expected: " << totalExpected << "\n";
    std::cout << "Total Diff:     " << (int64_t)(totalActual - totalExpected) << "\n";
    std::cout << "Expected Total: 119060324\n";
    std::cout << "Moves with differences: " << significantDiffs << "\n";
    
    if (!differences.empty()) {
        std::cout << "\n=== MOVES WITH DISCREPANCIES ===\n";
        for (const auto& [move, diff] : differences) {
            std::cout << move << ": " << (diff > 0 ? "+" : "") << diff << " nodes\n";
        }
        
        std::cout << "\n=== LARGEST DISCREPANCIES ===\n";
        std::sort(differences.begin(), differences.end(), 
                  [](const auto& a, const auto& b) { return abs(a.second) > abs(b.second); });
        
        for (size_t i = 0; i < std::min(5ULL, differences.size()); ++i) {
            const auto& [move, diff] = differences[i];
            uint64_t expected_val = expected[move];
            double percent = expected_val > 0 ? (100.0 * diff / expected_val) : 0.0;
            std::cout << move << ": " << (diff > 0 ? "+" : "") << diff 
                     << " (" << std::fixed << std::setprecision(3) << percent << "%)\n";
        }
    }
    
    return 0;
}
