#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

void debugG2G4Specific() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== BEFORE g2-g4 ===\n";
    std::cout << "En passant square: " << board.getEnPassantSquare() << "\n";
    
    // Check initial black pawns
    uint64_t blackPawns = board.getBlackPawns();
    std::cout << "Black pawn on h3: " << ((blackPawns & (1ULL << 23)) ? "YES" : "NO") << "\n";
    
    // Now apply the move
    Board::MoveState state;
    board.makeMove("g2-g4", state);
    
    std::cout << "\n=== AFTER g2-g4 ===\n";
    std::cout << "En passant square: " << board.getEnPassantSquare() << "\n";
    
    // Check if black pawn still on h3 after move
    blackPawns = board.getBlackPawns();
    std::cout << "Black pawn still on h3: " << ((blackPawns & (1ULL << 23)) ? "YES" : "NO") << "\n";
    
    // Now check if black can actually capture en passant
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(board, false); // false = black to move
    
    std::cout << "\nChecking for en passant moves in black's responses:\n";
    int enPassantCount = 0;
    for (auto move : moves) {
        if (moveSpecial(move) == 2) {
            std::cout << "  En passant move: " << decodeMove(move) << "\n";
            enPassantCount++;
        }
    }
    
    if (enPassantCount == 0) {
        std::cout << "  No en passant moves found\n";
        std::cout << "  This suggests the en passant square was not set, or en passant moves not generated\n";
    }
}

int main() {
    debugG2G4Specific();
    return 0;
}
