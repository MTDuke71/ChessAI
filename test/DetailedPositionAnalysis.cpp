#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::cout << "=== POSITION ANALYSIS ===\n";
    
    Board board;
    board.loadFEN("rnbqkbnr/ppppp1pp/8/8/4Pp2/8/PPPPKPPP/RNBQ1BNR w kq - 0 3");
    
    std::cout << "Position: rnbqkbnr/ppppp1pp/8/8/4Pp2/8/PPPPKPPP/RNBQ1BNR w kq - 0 3\n\n";
    
    // Show the board (simplified version)
    std::cout << "Board analysis for position with white king on e2\n";
    std::cout << "Black pawn on f4, white pawn on e4\n";
    
    // Test if e3 is attacked by black
    MoveGenerator gen;
    
    std::cout << "\n=== ATTACK ANALYSIS ===\n";
    std::cout << "Square e3 (square 20):\n";
    
    // Make the move e2-e3 and check if king is in check
    Board testBoard = board;
    Board::MoveState state;
    testBoard.makeMove("e2-e3", state);
    
    bool kingInCheck = gen.isKingInCheck(testBoard, true);
    std::cout << "After e2-e3, white king in check: " << (kingInCheck ? "YES" : "NO") << "\n";
    
    // Check what black pieces can attack e3
    std::cout << "\nBlack pieces that can attack e3:\n";
    
    // Generate all black moves to see if any target e3
    auto blackMoves = gen.generateAllMoves(testBoard, false);
    bool e3Attacked = false;
    
    for (auto move : blackMoves) {
        std::string moveStr = decodeMove(move);
        // Check if move targets e3
        if (moveStr.find("-e3") != std::string::npos || moveStr.find("xe3") != std::string::npos) {
            std::cout << "  " << moveStr << "\n";
            e3Attacked = true;
        }
    }
    
    if (!e3Attacked) {
        std::cout << "  No black pieces can attack e3\n";
    }
    
    testBoard.unmakeMove(state);
    
    // Now check the difference in expected vs actual perft
    std::cout << "\n=== PERFT COMPARISON ===\n";
    std::cout << "Expected total: 438 nodes\n";
    std::cout << "Expected moves: 22\n";
    std::cout << "Expected e2e3: NOT PRESENT\n\n";
    
    auto actualMoves = gen.generateAllMoves(board, true);
    int legalCount = 0;
    bool hasE2E3 = false;
    
    for (auto move : actualMoves) {
        std::string moveStr = decodeMove(move);
        if (board.isMoveLegal(moveStr)) {
            legalCount++;
            if (moveStr == "e2-e3") {
                hasE2E3 = true;
            }
        }
    }
    
    std::cout << "Actual legal moves: " << legalCount << "\n";
    std::cout << "Actual e2e3: " << (hasE2E3 ? "PRESENT" : "NOT PRESENT") << "\n";
    
    return 0;
}
