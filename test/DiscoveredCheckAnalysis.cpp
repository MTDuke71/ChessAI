#include <iostream>
#include "Board.h"
#include "FastMoveGenerator.h"

int main() {
    std::cout << "Discovered Check Analysis\n";
    std::cout << "=========================\n";
    
    Board board;
    
    // Recreate position: e2-e4 e7-e5 g1-f3 b8-c6
    std::vector<std::string> moves = {"e2-e4", "e7-e5", "g1-f3", "b8-c6"};
    for (const auto& moveStr : moves) {
        Board::MoveState state;
        board.makeMove(moveStr, state);
        std::cout << "Made move: " << moveStr << "\n";
    }
    
    FastMoveGenerator fastGen;
    
    std::cout << "\nBefore f2-f4:\n";
    bool kingInCheckBefore = fastGen.isKingInCheck(board, true);
    std::cout << "White king in check: " << (kingInCheckBefore ? "YES" : "NO") << "\n";
    
    // Print piece positions to understand the setup
    uint64_t whiteKing = board.getWhiteKing();
    uint64_t blackBishops = board.getBlackBishops();
    uint64_t blackQueens = board.getBlackQueens();
    
    // Convert bitboard to square indices
    auto printSquares = [](uint64_t bitboard, const std::string& name) {
        std::cout << name << ": ";
        for (int sq = 0; sq < 64; ++sq) {
            if (bitboard & (1ULL << sq)) {
                char file = 'a' + (sq % 8);
                char rank = '1' + (sq / 8);
                std::cout << file << rank << " ";
            }
        }
        std::cout << "\n";
    };
    
    printSquares(whiteKing, "White King");
    printSquares(blackBishops, "Black Bishops");
    printSquares(blackQueens, "Black Queens");
    
    // Make f2-f4 and check again
    std::cout << "\nMaking f2-f4...\n";
    Board::MoveState f2f4State;
    board.makeMove("f2-f4", f2f4State);
    
    bool kingInCheckAfter = fastGen.isKingInCheck(board, true);
    std::cout << "White king in check after f2-f4: " << (kingInCheckAfter ? "YES" : "NO") << "\n";
    
    // If king is in check, this means f2-f4 should be illegal
    if (kingInCheckAfter) {
        std::cout << "❌ f2-f4 puts White king in check - move should be ILLEGAL!\n";
        std::cout << "BUG: FastMoveGenerator legal filtering failed to catch this\n";
    } else {
        std::cout << "✅ f2-f4 does not put White king in check\n";
        std::cout << "This suggests the Original MoveGenerator might be wrong\n";
    }
    
    board.unmakeMove(f2f4State);
    
    return 0;
}
