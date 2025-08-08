#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <iomanip>

uint64_t perftWithDetails(Board& board, int depth, bool showDetails = false) {
    if (depth == 0) return 1;
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, board.isWhiteToMove());
    
    if (showDetails) {
        std::cout << "Position: " << board.getFEN() << "\n";
        std::cout << "Generated " << moves.size() << " moves:\n";
        for (uint16_t move : moves) {
            std::cout << "  " << decodeMove(move) << "\n";
        }
        std::cout << "\n";
    }
    
    uint64_t nodes = 0;
    for (uint16_t move : moves) {
        Board::MoveState state;
        board.makeMove(move, state);
        nodes += perftWithDetails(board, depth - 1, false);
        board.unmakeMove(state);
    }
    return nodes;
}

int main() {
    // Test the original position 3
    std::string fen = "4k3/8/8/8/8/8/8/4K2R w K - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== Testing Position 3 Rules ===\n";
    std::cout << "FEN: " << fen << "\n\n";
    
    board.printBoard();
    
    std::cout << "Board state details:\n";
    std::cout << "  White to move: " << (board.isWhiteToMove() ? "Yes" : "No") << "\n";
    std::cout << "  Halfmove clock: " << board.getHalfmoveClock() << "\n";
    std::cout << "  Fullmove number: " << board.getFullmoveNumber() << "\n";
    std::cout << "  En passant square: " << board.getEnPassantSquare() << "\n";
    std::cout << "  Can castle WK: " << (board.canCastleWK() ? "Yes" : "No") << "\n";
    std::cout << "  Can castle WQ: " << (board.canCastleWQ() ? "Yes" : "No") << "\n";
    std::cout << "  Can castle BK: " << (board.canCastleBK() ? "Yes" : "No") << "\n";
    std::cout << "  Can castle BQ: " << (board.canCastleBQ() ? "Yes" : "No") << "\n\n";
    
    // Test the first few moves to see if we're handling state correctly
    std::cout << "=== Testing first move: e1-g1 (castling) ===\n";
    
    uint16_t castlingMove = encodeMove("O-O", true);
    std::cout << "Castling move encoded as: " << castlingMove << "\n";
    std::cout << "Decoded back as: " << decodeMove(castlingMove) << "\n";
    std::cout << "Is castling legal: " << (board.isMoveLegal(castlingMove) ? "Yes" : "No") << "\n\n";
    
    if (board.isMoveLegal(castlingMove)) {
        Board::MoveState state;
        board.makeMove(castlingMove, state);
        
        std::cout << "After castling:\n";
        board.printBoard();
        std::cout << "FEN: " << board.getFEN() << "\n";
        std::cout << "Halfmove clock: " << board.getHalfmoveClock() << "\n";
        std::cout << "Fullmove number: " << board.getFullmoveNumber() << "\n\n";
        
        // Check perft from this position
        uint64_t perftResult = perftWithDetails(board, 5, true);
        std::cout << "Perft(5) from this position: " << perftResult << "\n";
        std::cout << "Expected: 26553\n";
        std::cout << "Difference: " << ((int64_t)perftResult - 26553) << "\n\n";
        
        board.unmakeMove(state);
    }
    
    // Also test a regular king move
    std::cout << "=== Testing regular king move: e1-f1 ===\n";
    uint16_t kingMove = encodeMove("e1-f1", true);
    std::cout << "King move encoded as: " << kingMove << "\n";
    std::cout << "Is king move legal: " << (board.isMoveLegal(kingMove) ? "Yes" : "No") << "\n\n";
    
    if (board.isMoveLegal(kingMove)) {
        Board::MoveState state;
        board.makeMove(kingMove, state);
        
        std::cout << "After e1-f1:\n";
        board.printBoard();
        std::cout << "FEN: " << board.getFEN() << "\n";
        std::cout << "Halfmove clock: " << board.getHalfmoveClock() << "\n";
        std::cout << "Fullmove number: " << board.getFullmoveNumber() << "\n\n";
        
        uint64_t perftResult = perftWithDetails(board, 5, false);
        std::cout << "Perft(5) from this position: " << perftResult << "\n";
        std::cout << "Expected: 42182\n";
        std::cout << "Difference: " << ((int64_t)perftResult - 42182) << "\n\n";
        
        board.unmakeMove(state);
    }
    
    return 0;
}
