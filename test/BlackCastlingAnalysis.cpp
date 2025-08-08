#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>

int main() {
    std::cout << "=== BLACK QUEENSIDE CASTLING ANALYSIS ===\n";
    
    // Position after a2-a3 where black can castle queenside
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q1p/1PPBBPPP/R3K2R b KQkq - 0 1");
    
    std::cout << "Starting position: ";
    std::cout << "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q1p/1PPBBPPP/R3K2R b KQkq - 0 1\n\n";
    
    // Make black queenside castling
    Board::MoveState state;
    board.makeMove("O-O-O", state);
    
    std::cout << "After black O-O-O, expected FEN should show:\n";
    std::cout << "- King on c8\n";
    std::cout << "- Rook from a8 moved to d8\n\n";
    
    // Print the board state
    std::cout << "Black king position: " << __builtin_ctzll(board.getBlackKing()) << "\n";
    std::cout << "Black rooks: " << std::hex << board.getBlackRooks() << std::dec << "\n";
    
    // Now count white's response moves (should be 50)
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(board, true); // White to move
    
    std::cout << "\nWhite moves after black O-O-O:\n";
    int legalMoves = 0;
    
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        if (board.isMoveLegal(moveStr)) {
            legalMoves++;
            std::cout << moveStr << " ";
            if (legalMoves % 10 == 0) std::cout << "\n";
        }
    }
    
    std::cout << "\n\nTotal legal white moves: " << legalMoves << "\n";
    std::cout << "Expected: 50\n";
    std::cout << "Difference: " << (legalMoves - 50) << "\n";
    
    // Check if the castling was executed correctly
    uint64_t expectedKingPos = 1ULL << 58; // c8 = square 58
    uint64_t expectedRookPos = 1ULL << 59; // d8 = square 59
    
    if (board.getBlackKing() & expectedKingPos) {
        std::cout << "✓ King correctly positioned on c8\n";
    } else {
        std::cout << "✗ King NOT on c8! Position: " << __builtin_ctzll(board.getBlackKing()) << "\n";
    }
    
    if (board.getBlackRooks() & expectedRookPos) {
        std::cout << "✓ Rook correctly positioned on d8\n";
    } else {
        std::cout << "✗ Rook NOT on d8! Rook positions: " << std::hex << board.getBlackRooks() << std::dec << "\n";
    }
    
    return 0;
}
