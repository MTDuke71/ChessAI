#include <iostream>
#include "../src/Board.h"
#include "../src/Engine.h"
#include "../src/MoveEncoding.h"

// Test helper function to simulate UCI castling conversion
std::string toUCIMove(const std::string& move, bool isWhiteToMove) {
    // Handle castling notation
    if (move == "O-O") {
        return isWhiteToMove ? "e1g1" : "e8g8";
    }
    if (move == "O-O-O") {
        return isWhiteToMove ? "e1c1" : "e8c8";
    }
    
    auto dash = move.find('-');
    if (dash == std::string::npos) return move;

    std::string from = move.substr(0, 2);
    std::string to = move.substr(dash + 1, 2);

    if (from == "e1" && to == "h1") to = "g1";
    else if (from == "e1" && to == "a1") to = "c1";
    else if (from == "e8" && to == "h8") to = "g8";
    else if (from == "e8" && to == "a8") to = "c8";

    std::string uci = from + to;

    if (move.size() > dash + 3) {
        char c = std::tolower(move.back());
        if (c == 'q' || c == 'r' || c == 'b' || c == 'n')
            uci += c;
    }
    return uci;
}

int main() {
    std::cout << "=== UCI CASTLING FIX TEST ===\n\n";
    
    // Test castling conversion
    std::cout << "1. Testing castling notation conversion:\n";
    std::cout << "   White O-O -> " << toUCIMove("O-O", true) << " (expected: e1g1)\n";
    std::cout << "   Black O-O -> " << toUCIMove("O-O", false) << " (expected: e8g8)\n";
    std::cout << "   White O-O-O -> " << toUCIMove("O-O-O", true) << " (expected: e1c1)\n";
    std::cout << "   Black O-O-O -> " << toUCIMove("O-O-O", false) << " (expected: e8c8)\n\n";
    
    // Test regular moves are unaffected
    std::cout << "2. Testing regular moves are unaffected:\n";
    std::cout << "   e2-e4 -> " << toUCIMove("e2-e4", true) << " (expected: e2e4)\n";
    std::cout << "   Nf3 -> " << toUCIMove("Nf3", true) << " (expected: Nf3)\n\n";
    
    // Test the actual engine castling behavior
    std::cout << "3. Testing engine castling generation:\n";
    Board board;
    
    // Set up a position where white can castle kingside
    board.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    std::cout << "   Position: r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1\n";
    std::cout << "   White to move, both sides can castle\n";
    
    Engine engine;
    std::string bestMove = engine.searchBestMove(board, 2);
    std::cout << "   Engine best move: " << bestMove << "\n";
    std::string uciMove = toUCIMove(bestMove, board.isWhiteToMove());
    std::cout << "   UCI conversion: " << uciMove << "\n\n";
    
    std::cout << "=== Test completed ===\n";
    return 0;
}
