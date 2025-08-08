#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <iomanip>

uint64_t perftDivide(Board& board, int depth, bool showMoves = false) {
    if (depth == 0) return 1;
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, board.isWhiteToMove());
    
    if (depth == 1) {
        if (showMoves) {
            std::cout << "Moves at depth 1:\n";
            for (uint16_t move : moves) {
                std::cout << "  " << decodeMove(move) << "\n";
            }
        }
        return moves.size();
    }
    
    uint64_t total = 0;
    for (uint16_t move : moves) {
        Board::MoveState state;
        board.makeMove(move, state);
        uint64_t count = perftDivide(board, depth - 1, false);
        total += count;
        if (showMoves && depth <= 3) {
            std::cout << decodeMove(move) << ": " << count << "\n";
        }
        board.unmakeMove(state);
    }
    return total;
}

int main() {
    // Original position 3
    std::string fen = "4k3/8/8/8/8/8/8/4K2R w K - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== Systematic Analysis of Position 3 ===\n";
    std::cout << "FEN: " << fen << "\n\n";
    
    board.printBoard();
    
    // First, let's check if the issue might be in the FEN parsing
    std::cout << "Loaded FEN details:\n";
    std::cout << "  White to move: " << (board.isWhiteToMove() ? "Yes" : "No") << "\n";
    std::cout << "  Castling rights: " << (board.canCastleWK() ? "K" : "") 
              << (board.canCastleWQ() ? "Q" : "") 
              << (board.canCastleBK() ? "k" : "") 
              << (board.canCastleBQ() ? "q" : "") << "\n";
    std::cout << "  En passant: " << board.getEnPassantSquare() << "\n";
    std::cout << "  Halfmove clock: " << board.getHalfmoveClock() << "\n";
    std::cout << "  Fullmove number: " << board.getFullmoveNumber() << "\n";
    std::cout << "  Regenerated FEN: " << board.getFEN() << "\n\n";
    
    // Check perft at different depths
    std::cout << "Perft results:\n";
    for (int d = 1; d <= 6; d++) {
        uint64_t result = perftDivide(board, d, d == 1);
        std::cout << "  D" << d << ": " << result << "\n";
    }
    
    std::cout << "\nExpected results:\n";
    std::cout << "  D1: 15\n";
    std::cout << "  D2: 66\n"; 
    std::cout << "  D3: 1197\n";
    std::cout << "  D4: 7059\n";
    std::cout << "  D5: 133987\n";
    std::cout << "  D6: 764643\n";
    
    // Let's also verify that our basic position is set up correctly
    std::cout << "\nPiece verification:\n";
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, true);
    std::cout << "Generated " << moves.size() << " moves:\n";
    for (uint16_t move : moves) {
        std::cout << "  " << decodeMove(move) << "\n";
    }
    
    return 0;
}
