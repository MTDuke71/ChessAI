#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/FastMoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>
#include <map>

// Expected perft values after d2-d3
std::map<std::string, uint64_t> expectedValues = {
    {"a7-a5", 15653}, {"a7-a6", 14176}, {"b7-b5", 15700}, {"b7-b6", 15660},
    {"b8-a6", 14899}, {"b8-c6", 16371}, {"c7-c5", 15971}, {"c7-c6", 15206},
    {"d7-d5", 20887}, {"d7-d6", 20079}, {"e7-e5", 21639}, {"e7-e6", 21624},
    {"f7-f5", 14882}, {"f7-f6", 14182}, {"g7-g5", 14753}, {"g7-g6", 15625},
    {"g8-f6", 16343}, {"g8-h6", 14876}, {"h7-h5", 15705}, {"h7-h6", 14280}
};

// Recursive perft for FastMoveGenerator
uint64_t perftFast(Board& board, int depth) {
    if (depth == 0) return 1;
    
    FastMoveGenerator fastGen;
    FastMoveGenerator::MoveList moves;
    fastGen.generateLegalMoves(board, board.isWhiteToMove(), moves);
    
    uint64_t nodes = 0;
    for (int i = 0; i < moves.count; i++) {
        Board::MoveState state;
        std::string moveStr = moves.moves[i].toAlgebraic();
        board.makeMove(moveStr, state);
        nodes += perftFast(board, depth - 1);
        board.unmakeMove(state);
    }
    
    return nodes;
}

int main() {
    Board board;
    FastMoveGenerator fastGen;
    
    // Load starting position and play d2-d3
    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    std::cout << "DEBUGGING d2-d3 PERFT DISCREPANCY\n";
    std::cout << "=================================\n\n";
    
    // Make the move d2-d3
    Board::MoveState d3State;
    board.makeMove("d2-d3", d3State);
    
    std::cout << "Position after d2-d3: " << board.getFEN() << "\n";
    std::cout << "Expected total at depth 4: 328,511\n\n";
    
    // Generate black's moves and test each one
    FastMoveGenerator::MoveList blackMoves;
    fastGen.generateLegalMoves(board, false, blackMoves); // Black to move
    
    std::cout << "BLACK MOVES PERFT DIVIDE (Depth 4):\n";
    std::cout << "===================================\n";
    
    uint64_t total = 0;
    bool foundError = false;
    
    for (int i = 0; i < blackMoves.count; i++) {
        Board tempBoard = board;
        std::string moveStr = blackMoves.moves[i].toAlgebraic();
        uint64_t nodes = perftFast(tempBoard, 3); // depth 3 since we already made 1 move
        total += nodes;
        
        auto it = expectedValues.find(moveStr);
        if (it != expectedValues.end()) {
            if (it->second != nodes) {
                std::cout << "❌ " << moveStr << ": " << nodes << " (expected " << it->second << ", diff: " << 
                    (int64_t)nodes - (int64_t)it->second << ")\n";
                foundError = true;
            } else {
                std::cout << "✅ " << moveStr << ": " << nodes << "\n";
            }
        } else {
            std::cout << "❓ " << moveStr << ": " << nodes << " (UNEXPECTED MOVE)\n";
            foundError = true;
        }
    }
    
    // Check for missing moves
    for (const auto& pair : expectedValues) {
        bool found = false;
        for (int i = 0; i < blackMoves.count; i++) {
            if (blackMoves.moves[i].toAlgebraic() == pair.first) {
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "❌ MISSING MOVE: " << pair.first << " (expected " << pair.second << " nodes)\n";
            foundError = true;
        }
    }
    
    std::cout << "\nTotal: " << total << " (expected 328,511, diff: " << (int64_t)total - 328511 << ")\n";
    
    if (!foundError && total == 328511) {
        std::cout << "\n🎉 ALL CORRECT! No discrepancy found.\n";
    } else {
        std::cout << "\n❌ Discrepancy found. FastMoveGenerator has bugs to fix.\n";
    }
    
    return 0;
}
