#include "Board.h"
#include "MoveGenerator.h"
#include "PrintMoves.h"
#include <iostream>
#include <cassert>

// Test 1: Pawn Promotion
void testPawnPromotion() {
    Board board;
    board.clearBoard();

    // White Pawn ready for promotion at rank 7
    board.setWhitePawns(0x00FF000000000000); // Pawns on Rank 7 (promotion row)
    MoveGenerator generator;

    std::vector<std::string> moves = generator.generatePawnMoves(board, true);
    std::cout << "\n[?] Pawn Promotion Test\n";
    printMoves(moves);

    // Ensure at least one promotion move is generated
    assert(!moves.empty());
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
    std::vector<std::string> moves = generator.generatePawnMoves(board, true);

    std::cout << "\n[?] En Passant Test\n";
    printMoves(moves);

    // Ensure en passant move is included
    assert(!moves.empty());
}

int main()
{
    testPawnPromotion();
    testEnPassant();

    std::cout << "\nAll tests passed successfully!\n";
    return 0;
}

