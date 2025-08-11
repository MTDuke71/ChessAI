#include "Board.h"
#include "Engine.h"
#include "MoveEncoding.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

void testUCICastlingMoves() {
    Board board;
    
    // Test black kingside castling (the original bug case)
    board.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b KQkq - 0 1");
    
    // Convert UCI move to internal format
    std::string uciMove = "e8g8";
    std::string from = uciMove.substr(0, 2);
    std::string to = uciMove.substr(2, 2);
    std::string internal = from + "-" + to;
    
    // Test that castling-aware encoding works
    uint16_t moveCode = encodeMove(internal, board.isWhiteToMove(), true);
    assert(board.isMoveLegal(moveCode));
    
    std::cout << "[✔] Black kingside castling (e8g8) is legal\n";
    
    // Test white kingside castling
    board.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    uciMove = "e1g1";
    from = uciMove.substr(0, 2);
    to = uciMove.substr(2, 2);
    internal = from + "-" + to;
    
    moveCode = encodeMove(internal, board.isWhiteToMove(), true);
    assert(board.isMoveLegal(moveCode));
    
    std::cout << "[✔] White kingside castling (e1g1) is legal\n";
    
    // Test white queenside castling
    uciMove = "e1c1";
    from = uciMove.substr(0, 2);
    to = uciMove.substr(2, 2);
    internal = from + "-" + to;
    
    moveCode = encodeMove(internal, board.isWhiteToMove(), true);
    assert(board.isMoveLegal(moveCode));
    
    std::cout << "[✔] White queenside castling (e1c1) is legal\n";
    
    // Test black queenside castling
    board.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b KQkq - 0 1");
    uciMove = "e8c8";
    from = uciMove.substr(0, 2);
    to = uciMove.substr(2, 2);
    internal = from + "-" + to;
    
    moveCode = encodeMove(internal, board.isWhiteToMove(), true);
    assert(board.isMoveLegal(moveCode));
    
    std::cout << "[✔] Black queenside castling (e8c8) is legal\n";
}

void testCastlingExecution() {
    Board board;
    
    // Test that black kingside castling actually moves pieces correctly
    board.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b KQkq - 0 1");
    
    // Execute the castling move
    std::string internal = "e8-g8";
    uint16_t moveCode = encodeMove(internal, board.isWhiteToMove(), true);
    assert(board.isMoveLegal(moveCode));
    
    board.makeMove(internal);
    
    // Verify king moved to g8 and rook moved to f8
    uint64_t blackKing = board.getBlackKing();
    uint64_t blackRooks = board.getBlackRooks();
    
    // King should be on g8 (square 62)
    assert(blackKing & (1ULL << 62));
    
    // Rook should be on f8 (square 61)
    assert(blackRooks & (1ULL << 61));
    
    // King should NOT be on e8 (square 60)
    assert(!(blackKing & (1ULL << 60)));
    
    // Rook should NOT be on h8 (square 63)
    assert(!(blackRooks & (1ULL << 63)));
    
    std::cout << "[✔] Black kingside castling executes correctly\n";
}

void testNonCastlingMovesUnaffected() {
    Board board;
    board.loadFEN("4k3/8/8/8/8/8/8/4K2R w K - 0 1");
    
    // Test that regular rook moves are not affected by castling detection
    std::string internal = "h1-h7";
    uint16_t moveCode = encodeMove(internal, board.isWhiteToMove(), false); // No castling detection
    assert(board.isMoveLegal(moveCode));
    
    std::cout << "[✔] Regular rook moves unaffected by castling detection\n";
    
    // Test that the same move with castling detection enabled still works
    moveCode = encodeMove(internal, board.isWhiteToMove(), true); // With castling detection
    assert(board.isMoveLegal(moveCode));
    
    std::cout << "[✔] Regular moves work with castling detection enabled\n";
}

int main() {
    std::cout << "Testing UCI Castling Regression...\n";
    
    testUCICastlingMoves();
    testCastlingExecution();
    testNonCastlingMovesUnaffected();
    
    std::cout << "All UCI castling tests passed!\n";
    return 0;
}
