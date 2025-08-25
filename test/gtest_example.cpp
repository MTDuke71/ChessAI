#include <gtest/gtest.h>
#include "Board.h"
#include "MoveGenerator.h"

// Test that Google Test framework is working
TEST(GoogleTestFramework, BasicTest) {
    EXPECT_EQ(2 + 2, 4);
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
}

// Test basic Board functionality with Google Test
TEST(BoardTest, InitialBoardSetup) {
    Board board;
    
    // Test that the board initializes with standard starting position
    EXPECT_TRUE(board.isWhiteToMove());
    
    // Test that white pawns are in the correct position
    uint64_t whitePawns = board.getWhitePawns();
    uint64_t expectedWhitePawns = 0x000000000000FF00ULL; // Rank 2
    EXPECT_EQ(whitePawns, expectedWhitePawns);
    
    // Test that black pawns are in the correct position  
    uint64_t blackPawns = board.getBlackPawns();
    uint64_t expectedBlackPawns = 0x00FF000000000000ULL; // Rank 7
    EXPECT_EQ(blackPawns, expectedBlackPawns);
}

// Test move generation with Google Test
TEST(MoveGeneratorTest, InitialMoveGeneration) {
    Board board;
    MoveGenerator generator;
    
    // Test that white has the expected number of initial moves
    std::vector<uint16_t> whiteMoves = generator.generateLegalMoves(board, true);
    EXPECT_EQ(whiteMoves.size(), 20); // Standard 20 opening moves for white
}

// Test FEN loading functionality
TEST(BoardTest, FENLoading) {
    Board board;
    std::string testFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    board.loadFEN(testFEN);
    EXPECT_TRUE(board.isWhiteToMove());
    
    // Verify the loaded position matches expectations
    std::string loadedFEN = board.getFEN();
    EXPECT_EQ(loadedFEN, testFEN);
}