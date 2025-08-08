#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

void debugEnPassant() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "Initial position:\n";
    board.printBoard();
    
    // Make the move a2-a4
    Board::MoveState state;
    board.makeMove("a2-a4", state);
    
    std::cout << "\nAfter a2-a4:\n";
    board.printBoard();
    std::cout << "En passant square: " << board.getEnPassantSquare() << "\n";
    
    // Check what's on b4 (should be black pawn that can capture en passant)
    uint64_t blackPawns = board.getBlackPawns();
    std::cout << "Black pawns bitboard: 0x" << std::hex << blackPawns << std::dec << "\n";
    
    // Check if there's a black pawn on b4 (square 25)
    if (blackPawns & (1ULL << 25)) {
        std::cout << "Black pawn on b4 found!\n";
        
        // Manually check if en passant should be possible
        int enPassantSquare = board.getEnPassantSquare();
        if (enPassantSquare == 16) { // a3
            std::cout << "En passant to a3 should be legal\n";
            
            // Test the move manually
            std::string enPassantMove = "b4-a3";
            uint16_t encodedMove = encodeMove(enPassantMove);
            std::cout << "Encoded move: " << encodedMove << "\n";
            std::cout << "Move special: " << moveSpecial(encodedMove) << "\n";
            
            if (board.isMoveLegal(enPassantMove)) {
                std::cout << "✅ En passant move is legal!\n";
            } else {
                std::cout << "❌ En passant move is NOT legal - this is the bug!\n";
            }
        }
    } else {
        std::cout << "No black pawn on b4\n";
    }
    
    // Also check if there's a black pawn that can capture from the right
    // In this position, there might be other en passant possibilities
    
    // List all black pawns
    std::cout << "\nAll black pawn positions:\n";
    for (int sq = 0; sq < 64; sq++) {
        if (blackPawns & (1ULL << sq)) {
            int file = sq % 8;
            int rank = sq / 8;
            char fileChar = 'a' + file;
            char rankChar = '1' + rank;
            std::cout << "Black pawn at " << fileChar << rankChar << " (square " << sq << ")\n";
        }
    }
}

int main() {
    debugEnPassant();
    return 0;
}
