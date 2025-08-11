#include <iostream>
#include "Board.h"
#include "FastMoveGenerator.h"

int main() {
    std::cout << "Sequential Move Generation Debug\n";
    std::cout << "=================================\n\n";
    
    Board board;
    FastMoveGenerator generator;
    FastMoveGenerator::MoveList moveList;
    
    // Generate WHITE moves from starting position
    std::cout << "=== WHITE moves from starting position ===\n";
    generator.generateLegalMoves(board, true, moveList);
    std::cout << "Generated " << moveList.count << " moves for WHITE:\n";
    for (int i = 0; i < std::min(5, moveList.count); ++i) {
        std::cout << "  " << moveList.moves[i].toAlgebraic() << "\n";
    }
    
    // Make the first move (e2-e4)
    std::cout << "\n=== Making move e2-e4 ===\n";
    Board::MoveState state;
    board.makeMove("e2-e4", state);
    
    // Generate BLACK moves after e2-e4
    std::cout << "\n=== BLACK moves after e2-e4 ===\n";
    generator.generateLegalMoves(board, false, moveList);
    std::cout << "Generated " << moveList.count << " moves for BLACK:\n";
    for (int i = 0; i < std::min(5, moveList.count); ++i) {
        auto move = moveList.moves[i];
        std::string moveStr = move.toAlgebraic();
        std::cout << "  " << moveStr;
        
        // Check if this is actually a black move
        char fromRank = moveStr[1];
        if (fromRank == '7' || fromRank == '8') {
            std::cout << " [CORRECT - BLACK piece]";
        } else if (fromRank == '2' || fromRank == '1') {
            std::cout << " [ERROR - WHITE piece!]";
        }
        std::cout << "\n";
    }
    
    // Generate WHITE moves again (should be empty or wrong)
    std::cout << "\n=== Trying to generate WHITE moves after e2-e4 (should fail) ===\n";
    generator.generateLegalMoves(board, true, moveList);
    std::cout << "Generated " << moveList.count << " moves for WHITE:\n";
    for (int i = 0; i < std::min(5, moveList.count); ++i) {
        auto move = moveList.moves[i];
        std::string moveStr = move.toAlgebraic();
        std::cout << "  " << moveStr;
        
        // Check if this is actually a white move
        char fromRank = moveStr[1];
        if (fromRank == '2' || fromRank == '1') {
            std::cout << " [CORRECT - WHITE piece]";
        } else if (fromRank == '7' || fromRank == '8') {
            std::cout << " [ERROR - BLACK piece!]";
        }
        std::cout << "\n";
    }
    
    return 0;
}
