#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

class Depth3Analyzer {
public:
    static void analyzeDepth3Discrepancies() {
        std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
        
        Board board;
        board.loadFEN(fen);
        
        std::cout << "=== DEPTH 3 DISCREPANCY ANALYSIS ===\n";
        std::cout << "Expected: 97862, Getting: 97330, Missing: 532\n\n";
        
        MoveGenerator gen;
        auto whiteMoves = gen.generateAllMoves(board, true);
        
        std::map<std::string, int> moveDiscrepancies;
        uint64_t totalCalculated = 0;
        
        // Expected values for each white move at depth 3 (from reference engines)
        std::map<std::string, uint64_t> expectedD3Values = {
            {"a2-a3", 2182}, {"a2-a4", 2145}, {"b2-b3", 1970}, {"g2-g3", 1961},
            {"g2-g4", 1957}, {"g2-h3", 2021}, {"d5-d6", 1958}, {"d5-e6", 2237},
            {"c3-a4", 2199}, {"c3-b1", 1970}, {"c3-b5", 2134}, {"c3-d1", 1970},
            {"e5-c4", 1957}, {"e5-c6", 1958}, {"e5-d3", 2021}, {"e5-d7", 2122},
            {"e5-f7", 2078}, {"e5-g4", 2021}, {"e5-g6", 1961}, {"d2-c1", 2001},
            {"d2-e3", 2129}, {"d2-f4", 2001}, {"d2-g5", 2127}, {"d2-h6", 1958},
            {"e2-a6", 1538}, {"e2-b5", 2051}, {"e2-c4", 2076}, {"e2-d1", 2218},
            {"e2-d3", 1970}, {"e2-f1", 2057}, {"a1-b1", 2001}, {"a1-c1", 2001},
            {"a1-d1", 2001}, {"h1-f1", 2001}, {"h1-g1", 2001}, {"f3-d3", 1970},
            {"f3-e3", 2170}, {"f3-f4", 2128}, {"f3-f5", 2392}, {"f3-f6", 2107},
            {"f3-g3", 2210}, {"f3-g4", 2165}, {"f3-h3", 2356}, {"f3-h5", 2263},
            {"e1-d1", 2001}, {"e1-f1", 2001}, {"O-O", 2001}, {"O-O-O", 2001}
        };
        
        for (auto whiteMove : whiteMoves) {
            std::string moveStr = decodeMove(whiteMove);
            if (!board.isMoveLegal(moveStr)) continue;
            
            Board copy = board;
            Board::MoveState state;
            copy.makeMove(moveStr, state);
            
            // Calculate depth 2 from this position (our depth 3 contribution)
            uint64_t calculated = calculateDepth2(copy, gen);
            totalCalculated += calculated;
            
            // Compare with expected
            auto expectedIt = expectedD3Values.find(moveStr);
            if (expectedIt != expectedD3Values.end()) {
                int difference = (int)calculated - (int)expectedIt->second;
                if (difference != 0) {
                    moveDiscrepancies[moveStr] = difference;
                    std::cout << moveStr << ": calculated=" << calculated 
                              << ", expected=" << expectedIt->second 
                              << ", diff=" << difference << "\n";
                }
            } else {
                std::cout << "Unknown move: " << moveStr << " = " << calculated << "\n";
            }
        }
        
        std::cout << "\nTotal calculated: " << totalCalculated << "\n";
        std::cout << "Total expected: 97862\n";
        std::cout << "Overall difference: " << (int64_t)totalCalculated - 97862 << "\n";
        
        // Analyze patterns in discrepancies
        int positiveDiffs = 0, negativeDiffs = 0;
        for (const auto& [move, diff] : moveDiscrepancies) {
            if (diff > 0) positiveDiffs++;
            else if (diff < 0) negativeDiffs++;
        }
        
        std::cout << "\nDiscrepancy pattern:\n";
        std::cout << "Moves with positive differences: " << positiveDiffs << "\n";
        std::cout << "Moves with negative differences: " << negativeDiffs << "\n";
        std::cout << "Moves with exact matches: " << (whiteMoves.size() - moveDiscrepancies.size()) << "\n";
    }
    
private:
    static uint64_t calculateDepth2(const Board& board, MoveGenerator& gen) {
        auto moves = gen.generateAllMoves(board, board.isWhiteToMove());
        uint64_t total = 0;
        
        for (auto move : moves) {
            std::string moveStr = decodeMove(move);
            if (!board.isMoveLegal(moveStr)) continue;
            
            Board copy = board;
            Board::MoveState state;
            copy.makeMove(moveStr, state);
            
            auto responseMoves = gen.generateAllMoves(copy, copy.isWhiteToMove());
            for (auto responseMove : responseMoves) {
                std::string responseStr = decodeMove(responseMove);
                if (copy.isMoveLegal(responseStr)) {
                    total++;
                }
            }
        }
        
        return total;
    }
};

int main() {
    Depth3Analyzer::analyzeDepth3Discrepancies();
    return 0;
}
