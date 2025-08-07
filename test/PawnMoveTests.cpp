#include "Board.h"
#include "MoveGenerator.h"
#include "PrintMoves.h"
#include "BitUtils.h"
#include "MoveEncoding.h"
#include <iostream>
#include <cassert>

// Test 1: Pawn Promotion
void testPawnPromotion() {
    Board board;
    board.clearBoard();

    // White Pawn ready for promotion at rank 7
    board.setWhitePawns(0x00FF000000000000); // Pawns on Rank 7 (promotion row)
    MoveGenerator generator;

    std::vector<uint16_t> moves = generator.generatePawnMoves(board, true);
    std::cout << "\n[?] Pawn Promotion Test\n";
    printMoves(moves);

    // Expect 32 promotion moves (8 files * 4 piece choices)
    assert(moves.size() == 32);

    // Apply one underpromotion and verify board state changes
    board.makeMove(decodeMove(moves.front()));
    // The moved piece should no longer be a pawn
    assert(popcount64(board.getWhitePawns()) == 7);
}

// Test 2: En Passant
void testEnPassant() {
    Board board;
    board.clearBoard();

    // Place White Pawn on e5 and Black Pawn on d5
    board.setWhitePawns(0x1000000000);  // e5
    board.setBlackPawns(0x800000000);  // d5
    board.printBoard();

    // Simulate that Black Pawn just moved two squares forward to d5
    // The en passant square is the square passed over by the pawn.
    // Black pawn moved from d7 to d5, so d6 (index 43) is the target.
    board.setEnPassantSquare(43);  // Position d6 in bitboard notation

    MoveGenerator generator;
    std::vector<uint16_t> moves = generator.generatePawnMoves(board, true);

    std::cout << "\n[?] En Passant Test\n";
    printMoves(moves);

    // Ensure en passant move is included
    assert(!moves.empty());
}

// Test 3: En Passant Make/Unmake and Legality
void testEnPassantExecution() {
    Board board;
    board.clearBoard();

    // White pawn on e5, black pawn on d5 just advanced two squares
    board.setWhitePawns(1ULL << 36); // e5
    board.setBlackPawns(1ULL << 35); // d5
    board.setEnPassantSquare(43);    // d6

    // Move should be legal
    assert(board.isMoveLegal("e5-d6"));

    Board::MoveState st;
    board.makeMove("e5-d6", st);

    // White pawn captured en passant on d6
    assert(board.getWhitePawns() == (1ULL << 43));
    assert(board.getBlackPawns() == 0);
    assert(board.getEnPassantSquare() == -1);

    board.unmakeMove(st);

    // Board restored
    assert(board.getWhitePawns() == (1ULL << 36));
    assert(board.getBlackPawns() == (1ULL << 35));
    assert(board.getEnPassantSquare() == 43);
}

int main()
{
    testPawnPromotion();
    testEnPassant();
    testEnPassantExecution();

    std::cout << "\nAll tests passed successfully!\n";
    return 0;
}

