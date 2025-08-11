#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>

void writeNewDebugFile(const std::string& fen, int depth, const std::map<std::string, uint64_t>& moveNodes, uint64_t totalNodes) {
    std::ofstream file("../test/PeftDebug.txt");
    file << fen << "\n\n";
    file << "Depth: " << depth << "\n\n";
    file << "Total: " << totalNodes << "\n";
    
    for (const auto& [move, nodes] : moveNodes) {
        file << move << " - " << nodes << "\n";
    }
    
    file.close();
    std::cout << "Updated PeftDebug.txt with new position at depth " << depth << "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <move>\n";
        std::cout << "Example: " << argv[0] << " e2e4\n";
        std::cout << "This will make the move and generate expected results for the next depth\n";
        return 1;
    }
    
    std::string moveToMake = argv[1];
    std::cout << "=== DRILL DOWN ANALYZER ===\n";
    std::cout << "Making move: " << moveToMake << "\n\n";
    
    // Read current position from PeftDebug.txt
    std::ifstream currentFile("../test/PeftDebug.txt");
    std::string currentFen;
    int currentDepth = 0;
    
    std::string line;
    // Read FEN (first non-empty line)
    while (std::getline(currentFile, line)) {
        if (!line.empty() && (line.find("rnbqkbnr") != std::string::npos || line.find("/") != std::string::npos)) {
            currentFen = line;
            break;
        }
    }
    
    // Find depth
    while (std::getline(currentFile, line)) {
        if (line.find("Depth:") != std::string::npos) {
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string depthStr = line.substr(colonPos + 1);
                depthStr.erase(std::remove_if(depthStr.begin(), depthStr.end(), ::isspace), depthStr.end());
                currentDepth = std::stoi(depthStr);
                break;
            }
        }
    }
    currentFile.close();
    
    if (currentFen.empty() || currentDepth == 0) {
        std::cerr << "Error: Could not read current position from PeftDebug.txt\n";
        return 1;
    }
    
    std::cout << "Current position: " << currentFen << "\n";
    std::cout << "Current depth: " << currentDepth << "\n\n";
    
    if (currentDepth <= 1) {
        std::cerr << "Error: Already at minimum depth\n";
        return 1;
    }
    
    // Load the current position
    Board board;
    board.loadFEN(currentFen);
    
    // Verify the move is legal
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(board, board.isWhiteToMove());
    bool moveFound = false;
    
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        // Try both formats (with and without dashes)
        std::string normalizedMove = moveStr;
        normalizedMove.erase(std::remove(normalizedMove.begin(), normalizedMove.end(), '-'), normalizedMove.end());
        
        if (moveStr == moveToMake || normalizedMove == moveToMake) {
            moveFound = true;
            break;
        }
    }
    
    if (!moveFound) {
        std::cerr << "Error: Move '" << moveToMake << "' is not legal in current position\n";
        std::cout << "Legal moves: ";
        for (auto move : moves) {
            std::string moveStr = decodeMove(move);
            if (board.isMoveLegal(moveStr)) {
                std::string normalizedMove = moveStr;
                normalizedMove.erase(std::remove(normalizedMove.begin(), normalizedMove.end(), '-'), normalizedMove.end());
                std::cout << normalizedMove << " ";
            }
        }
        std::cout << "\n";
        return 1;
    }
    
    // Make the move
    Board::MoveState state;
    std::string moveWithDashes = moveToMake;
    if (moveToMake.length() == 4) {
        moveWithDashes = moveToMake.substr(0, 2) + "-" + moveToMake.substr(2, 2);
    }
    
    board.makeMove(moveWithDashes, state);
    std::string newFen = board.getFEN();
    int newDepth = currentDepth - 1;
    
    std::cout << "New position: " << newFen << "\n";
    std::cout << "New depth: " << newDepth << "\n";
    std::cout << "Generating perft divide data...\n\n";
    
    // Generate new move breakdown
    auto newMoves = gen.generateAllMoves(board, board.isWhiteToMove());
    std::map<std::string, uint64_t> newMoveNodes;
    uint64_t totalNodes = 0;
    
    for (auto move : newMoves) {
        std::string moveStr = decodeMove(move);
        
        // Make the move to check if it's legal
        Board::MoveState testState;
        Board testBoard = board;
        testBoard.makeMove(moveStr, testState);
        
        // Only count legal moves (not in check)
        if (!gen.isKingInCheck(testBoard, !testBoard.isWhiteToMove())) {
            std::cout << "Computing " << moveStr << "..." << std::flush;
            
            uint64_t nodes = perft(testBoard, gen, newDepth - 1);
            
            // Normalize move format (remove dashes)
            std::string normalizedMove = moveStr;
            normalizedMove.erase(std::remove(normalizedMove.begin(), normalizedMove.end(), '-'), normalizedMove.end());
            
            newMoveNodes[normalizedMove] = nodes;
            totalNodes += nodes;
            
            std::cout << "\r" << normalizedMove << " - " << nodes << " nodes\n";
        }
    }
    
    std::cout << "\nTotal nodes: " << totalNodes << "\n";
    std::cout << "Total moves: " << newMoveNodes.size() << "\n\n";
    
    // Write the new debug file
    writeNewDebugFile(newFen, newDepth, newMoveNodes, totalNodes);
    
    std::cout << "=== NEXT STEPS ===\n";
    std::cout << "1. Run: .\\PerftDifferencesAnalyzer.exe\n";
    std::cout << "2. Check pertdifferences.txt for new discrepancies\n";
    std::cout << "3. Choose next move to drill down further\n";
    std::cout << "4. Run: .\\DrillDownAnalyzer.exe <next-move>\n";
    
    return 0;
}
