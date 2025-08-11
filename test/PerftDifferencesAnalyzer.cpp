#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

struct PerftResult {
    std::string fen;
    int depth;
    uint64_t totalNodes;
    std::unordered_map<std::string, uint64_t> moveNodes;
};

PerftResult parsePerftDebugFile(const std::string& filename) {
    PerftResult result;
    std::ifstream file(filename);
    std::string line;
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return result;
    }
    
    // Read FEN (first non-empty line that looks like a FEN)
    while (std::getline(file, line)) {
        if (!line.empty() && 
            (line.find("/") != std::string::npos && 
             (line.find(" w ") != std::string::npos || line.find(" b ") != std::string::npos))) {
            result.fen = line;
            break;
        }
    }
    
    // Skip empty lines and find depth
    while (std::getline(file, line)) {
        if (line.find("Depth:") != std::string::npos) {
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string depthStr = line.substr(colonPos + 1);
                // Remove whitespace
                depthStr.erase(std::remove_if(depthStr.begin(), depthStr.end(), ::isspace), depthStr.end());
                if (!depthStr.empty()) {
                    result.depth = std::stoi(depthStr);
                    break;
                }
            }
        }
    }
    
    // Skip empty lines and find total
    while (std::getline(file, line)) {
        if (line.find("Total:") != std::string::npos) {
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string totalStr = line.substr(colonPos + 1);
                // Remove whitespace
                totalStr.erase(std::remove_if(totalStr.begin(), totalStr.end(), ::isspace), totalStr.end());
                result.totalNodes = std::stoull(totalStr);
                break;
            }
        }
    }
    
    // Read move-specific results
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        size_t dashPos = line.find(" - ");
        if (dashPos != std::string::npos) {
            std::string move = line.substr(0, dashPos);
            std::string nodesStr = line.substr(dashPos + 3);
            uint64_t nodes = std::stoull(nodesStr);
            result.moveNodes[move] = nodes;
        }
    }
    
    file.close();
    return result;
}

PerftResult generateActualPerft(const std::string& fen, int depth) {
    PerftResult result;
    result.fen = fen;
    result.depth = depth;
    
    Board board;
    if (!fen.empty()) {
        board.loadFEN(fen);
    }
    
    MoveGenerator generator;
    
    // Get total nodes using perft
    result.totalNodes = perft(board, generator, depth);
    
    // Get individual move nodes using the same logic as perftDivide
    auto moves = generator.generateAllMoves(board, board.isWhiteToMove());
    
    for (auto move : moves) {
        // Convert move to string format
        std::string moveStr = decodeMove(move);
        
        // Normalize move format (remove dashes to match expected format)
        std::string normalizedMove = moveStr;
        normalizedMove.erase(std::remove(normalizedMove.begin(), normalizedMove.end(), '-'), normalizedMove.end());
        
        // Make the move
        Board::MoveState state;
        board.makeMove(moveStr, state);
        
        // Only count legal moves (not in check)
        if (!generator.isKingInCheck(board, !board.isWhiteToMove())) {
            // Calculate perft for this move
            uint64_t nodes = perft(board, generator, depth - 1);
            result.moveNodes[normalizedMove] = nodes;
        }
        
        // Unmake the move
        board.unmakeMove(state);
    }
    
    return result;
}

void writePerftDifferences(const PerftResult& expected, const PerftResult& actual, const std::string& filename) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not create " << filename << std::endl;
        return;
    }
    
    file << "Perft Differences Analysis\n";
    file << "=========================\n\n";
    file << "Position: " << expected.fen << "\n";
    file << "Depth: " << expected.depth << "\n\n";
    
    file << "TOTAL NODES COMPARISON:\n";
    file << "Expected: " << expected.totalNodes << "\n";
    file << "Actual:   " << actual.totalNodes << "\n";
    file << "Difference: " << (int64_t)actual.totalNodes - (int64_t)expected.totalNodes << "\n\n";
    
    // Check for missing moves (in expected but not in actual)
    file << "MISSING MOVES (in expected but not generated):\n";
    bool foundMissing = false;
    for (const auto& [move, expectedNodes] : expected.moveNodes) {
        if (actual.moveNodes.find(move) == actual.moveNodes.end()) {
            file << "  " << move << " - " << expectedNodes << " nodes (MISSING)\n";
            foundMissing = true;
        }
    }
    if (!foundMissing) {
        file << "  None\n";
    }
    file << "\n";
    
    // Check for extra moves (in actual but not in expected)
    file << "EXTRA MOVES (generated but not in expected):\n";
    bool foundExtra = false;
    for (const auto& [move, actualNodes] : actual.moveNodes) {
        if (expected.moveNodes.find(move) == expected.moveNodes.end()) {
            file << "  " << move << " - " << actualNodes << " nodes (EXTRA)\n";
            foundExtra = true;
        }
    }
    if (!foundExtra) {
        file << "  None\n";
    }
    file << "\n";
    
    // Check for node count differences
    file << "NODE COUNT DIFFERENCES (same move, different count):\n";
    bool foundDifferences = false;
    for (const auto& [move, expectedNodes] : expected.moveNodes) {
        auto actualIt = actual.moveNodes.find(move);
        if (actualIt != actual.moveNodes.end()) {
            uint64_t actualNodes = actualIt->second;
            if (expectedNodes != actualNodes) {
                int64_t diff = (int64_t)actualNodes - (int64_t)expectedNodes;
                file << "  " << move << " - Expected: " << expectedNodes 
                     << ", Actual: " << actualNodes << ", Diff: " << diff << "\n";
                foundDifferences = true;
            }
        }
    }
    if (!foundDifferences) {
        file << "  None\n";
    }
    file << "\n";
    
    // Summary statistics
    file << "SUMMARY:\n";
    file << "Expected total moves: " << expected.moveNodes.size() << "\n";
    file << "Actual total moves: " << actual.moveNodes.size() << "\n";
    file << "Move difference: " << (int)actual.moveNodes.size() - (int)expected.moveNodes.size() << "\n";
    
    uint64_t expectedSum = 0, actualSum = 0;
    for (const auto& [move, nodes] : expected.moveNodes) {
        expectedSum += nodes;
    }
    for (const auto& [move, nodes] : actual.moveNodes) {
        actualSum += nodes;
    }
    file << "Expected sum of moves: " << expectedSum << "\n";
    file << "Actual sum of moves: " << actualSum << "\n";
    file << "Sum difference: " << (int64_t)actualSum - (int64_t)expectedSum << "\n";
    
    file.close();
    std::cout << "Perft differences written to " << filename << std::endl;
}

int main() {
    std::cout << "Perft Differences Analyzer\n";
    std::cout << "==========================\n\n";
    
    // Parse expected results from PeftDebug.txt
    std::cout << "Reading expected results from PeftDebug.txt...\n";
    PerftResult expected = parsePerftDebugFile("../test/PeftDebug.txt");
    
    if (expected.fen.empty()) {
        std::cerr << "Error: Could not parse PeftDebug.txt\n";
        return 1;
    }
    
    std::cout << "Position: " << expected.fen << "\n";
    std::cout << "Depth: " << expected.depth << "\n";
    std::cout << "Expected total: " << expected.totalNodes << "\n";
    std::cout << "Expected moves: " << expected.moveNodes.size() << "\n\n";
    
    // Generate actual results with FastMoveGenerator
    std::cout << "Generating actual results with FastMoveGenerator...\n";
    PerftResult actual = generateActualPerft(expected.fen, expected.depth);
    
    std::cout << "Actual total: " << actual.totalNodes << "\n";
    std::cout << "Actual moves: " << actual.moveNodes.size() << "\n\n";
    
    // Write differences to file
    std::cout << "Analyzing differences...\n";
    writePerftDifferences(expected, actual, "pertdifferences.txt");
    
    // Print summary to console
    int64_t totalDiff = (int64_t)actual.totalNodes - (int64_t)expected.totalNodes;
    int moveDiff = (int)actual.moveNodes.size() - (int)expected.moveNodes.size();
    
    std::cout << "\nSUMMARY:\n";
    std::cout << "Total node difference: " << totalDiff << "\n";
    std::cout << "Move count difference: " << moveDiff << "\n";
    
    if (totalDiff == 0 && moveDiff == 0) {
        std::cout << "✓ Perfect match! No differences found.\n";
    } else {
        std::cout << "✗ Differences found. Check pertdifferences.txt for details.\n";
    }
    
    return 0;
}
