#include "Board.h"
#include <iostream>
#include <sstream>
#include <cassert>

// Utility to capture `printBoard()` output for comparison
std::string captureBoardOutput(const Board& board) {
    std::ostringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    board.printBoard();
    std::cout.rdbuf(old);  // Restore original output stream
    return buffer.str();
}

// Test Case 1: Initial Board Setup
void testInitialBoard() {
    Board board;
    std::string expectedOutput =
        "   a b c d e f g h\n"
        "  +-----------------+\n"
        " 8| r n b q k b n r | 8\n"
        " 7| p p p p p p p p | 7\n"
        " 6| . . . . . . . . | 6\n"
        " 5| . . . . . . . . | 5\n"
        " 4| . . . . . . . . | 4\n"
        " 3| . . . . . . . . | 3\n"
        " 2| P P P P P P P P | 2\n"
        " 1| R N B Q K B N R | 1\n"
        "  +-----------------+\n"
        "   a b c d e f g h\n\n";
    
    assert(captureBoardOutput(board) == expectedOutput);
    std::cout << "[✔] Initial Board Test Passed\n";
}

// Test Case 2: Empty Board
void testEmptyBoard() {
    Board board;
    board.clearBoard();

    std::string expectedOutput =
        "   a b c d e f g h\n"
        "  +-----------------+\n"
        " 8| . . . . . . . . | 8\n"
        " 7| . . . . . . . . | 7\n"
        " 6| . . . . . . . . | 6\n"
        " 5| . . . . . . . . | 5\n"
        " 4| . . . . . . . . | 4\n"
        " 3| . . . . . . . . | 3\n"
        " 2| . . . . . . . . | 2\n"
        " 1| . . . . . . . . | 1\n"
        "  +-----------------+\n"
        "   a b c d e f g h\n\n";

    assert(captureBoardOutput(board) == expectedOutput);
    std::cout << "[✔] Empty Board Test Passed\n";
}

// Test Case 3: Custom Board Configuration
void testCustomBoard() {
    Board board;
    board.clearBoard();  // Clear the default board to ensure we control setup

    // Custom piece placement
    board.setWhitePawns(0x0000000000001000);  // Correct position for e2
    board.setBlackKing(0x0000000000000001);    // Black King on a1
    board.setWhiteRooks(0x0000000000000080);   // White Rook on h1
    board.setBlackQueens(0x0800000000000000);  // Black Queen on d8

    std::string expectedOutput =
        "   a b c d e f g h\n"
        "  +-----------------+\n"
        " 8| . . . q . . . . | 8\n"
        " 7| . . . . . . . . | 7\n"
        " 6| . . . . . . . . | 6\n"
        " 5| . . . . . . . . | 5\n"
        " 4| . . . . . . . . | 4\n"
        " 3| . . . . . . . . | 3\n"
        " 2| . . . . P . . . | 2\n"
        " 1| k . . . . . . R | 1\n"
        "  +-----------------+\n"
        "   a b c d e f g h\n\n";

    std::string actualOutput = captureBoardOutput(board);

    if (actualOutput != expectedOutput) {
        std::cout << "[❌] Custom Board Test Failed\n";
        std::cout << "EXPECTED OUTPUT:\n" << expectedOutput;
        std::cout << "ACTUAL OUTPUT:\n" << actualOutput;
        assert(false);  // Force assertion failure for debugging clarity
    }

    std::cout << "[✔] Custom Board Test Passed\n";
}

// Main Testing Function
int main() {
    testInitialBoard();
    testEmptyBoard();
    testCustomBoard();

    std::cout << "\nAll tests passed successfully!\n";
    return 0;
}
