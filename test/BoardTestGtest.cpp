#include <gtest/gtest.h>
#include "Board.h"
#include <iostream>
#include <sstream>

// Utility to capture `printBoard()` output for comparison (from original BoardTest.cpp)
std::string captureBoardOutput(const Board& board) {
    std::ostringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    board.printBoard();
    std::cout.rdbuf(old);  // Restore original output stream
    return buffer.str();
}

// Test Case 1: Initial Board Setup (converted from original BoardTest.cpp)
TEST(BoardTestGtest, InitialBoardSetup) {
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

    std::string actualOutput = captureBoardOutput(board);
    EXPECT_EQ(actualOutput, expectedOutput);
}

// Test Case 2: Empty Board (converted from original BoardTest.cpp)
TEST(BoardTestGtest, EmptyBoard) {
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

    std::string actualOutput = captureBoardOutput(board);
    EXPECT_EQ(actualOutput, expectedOutput);
}

// Test Case 3: Custom Board Configuration (converted from original BoardTest.cpp)
TEST(BoardTestGtest, CustomBoard) {
    Board board;
    board.clearBoard();

    // Place a white pawn on e2 and a black king on a1, white rook on h1
    board.setWhitePawns(1ULL << 12); // e2 = square 12
    board.setBlackKing(1ULL << 0);   // a1 = square 0
    board.setWhiteRooks(1ULL << 7);  // h1 = square 7

    std::string expectedOutput =
        "   a b c d e f g h\n"
        "  +-----------------+\n"
        " 8| . . . . . . . . | 8\n"
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
    EXPECT_EQ(actualOutput, expectedOutput);
}

// Additional Google Test specific tests
TEST(BoardTestGtest, FENLoadingAndExtraction) {
    Board board;
    std::string testFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    board.loadFEN(testFEN);
    
    // Test that the board is correctly loaded
    EXPECT_TRUE(board.isWhiteToMove());
    
    // Verify the FEN round trip
    std::string loadedFEN = board.getFEN();
    EXPECT_EQ(loadedFEN, testFEN);
}

// Test using Google Test's EXPECT_* macros for better error reporting
TEST(BoardTestGtest, PiecePositions) {
    Board board;
    
    // Test initial white pawn positions
    uint64_t whitePawns = board.getWhitePawns();
    uint64_t expectedWhitePawns = 0x000000000000FF00ULL; // Rank 2
    EXPECT_EQ(whitePawns, expectedWhitePawns);
    
    // Test initial black pawn positions  
    uint64_t blackPawns = board.getBlackPawns();
    uint64_t expectedBlackPawns = 0x00FF000000000000ULL; // Rank 7
    EXPECT_EQ(blackPawns, expectedBlackPawns);
    
    // Test king positions
    uint64_t whiteKing = board.getWhiteKing();
    uint64_t expectedWhiteKing = 1ULL << 4; // e1 = square 4
    EXPECT_EQ(whiteKing, expectedWhiteKing);
    
    uint64_t blackKing = board.getBlackKing();
    uint64_t expectedBlackKing = 1ULL << 60; // e8 = square 60
    EXPECT_EQ(blackKing, expectedBlackKing);
}