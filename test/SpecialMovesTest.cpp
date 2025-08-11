#include <iostream>
#include "Board.h"
#include "FastMoveGenerator.h"

int main() {
    std::cout << "Testing En Passant and Castling in FastMoveGenerator\n";
    std::cout << "===================================================\n\n";
    
    FastMoveGenerator gen;
    
    // Test 1: En Passant situation
    std::cout << "1. Testing En Passant:\n";
    Board enPassantBoard;
    // Position where en passant is possible: white pawn on e5, black just played d7-d5
    enPassantBoard.loadFEN("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");
    
    FastMoveGenerator::MoveList enPassantMoves;
    gen.generateLegalMoves(enPassantBoard, true, enPassantMoves);
    
    std::cout << "En passant position FEN: " << enPassantBoard.getFEN() << "\n";
    std::cout << "White moves available: " << enPassantMoves.count << "\n";
    
    // Look for en passant moves
    bool foundEnPassant = false;
    for (int i = 0; i < enPassantMoves.count; ++i) {
        std::string move = enPassantMoves.moves[i].toAlgebraic();
        if (move == "e5-d6" || move == "e5-f6") {
            std::cout << "  Found en passant: " << move << "\n";
            foundEnPassant = true;
        }
    }
    
    if (!foundEnPassant) {
        std::cout << "  ERROR: En passant move not found!\n";
    }
    
    std::cout << "\n2. Testing Castling:\n";
    Board castlingBoard;
    // Position where both castling moves are available
    castlingBoard.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    
    FastMoveGenerator::MoveList castlingMoves;
    gen.generateLegalMoves(castlingBoard, true, castlingMoves);
    
    std::cout << "Castling position FEN: " << castlingBoard.getFEN() << "\n";
    std::cout << "White moves available: " << castlingMoves.count << "\n";
    
    // Look for castling moves
    bool foundKingSide = false, foundQueenSide = false;
    for (int i = 0; i < castlingMoves.count; ++i) {
        std::string move = castlingMoves.moves[i].toAlgebraic();
        if (move == "e1-g1") {
            std::cout << "  Found king-side castling: " << move << "\n";
            foundKingSide = true;
        }
        if (move == "e1-c1") {
            std::cout << "  Found queen-side castling: " << move << "\n";
            foundQueenSide = true;
        }
    }
    
    if (!foundKingSide) {
        std::cout << "  ERROR: King-side castling not found!\n";
    }
    if (!foundQueenSide) {
        std::cout << "  ERROR: Queen-side castling not found!\n";
    }
    
    std::cout << "\n3. Testing position where castling should NOT be available:\n";
    Board noCastlingBoard;
    // King has moved (no castling rights)
    noCastlingBoard.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w - - 0 1");
    
    FastMoveGenerator::MoveList noCastlingMoves;
    gen.generateLegalMoves(noCastlingBoard, true, noCastlingMoves);
    
    std::cout << "No castling position FEN: " << noCastlingBoard.getFEN() << "\n";
    std::cout << "White moves available: " << noCastlingMoves.count << "\n";
    
    // Check for incorrect castling moves
    bool incorrectCastling = false;
    for (int i = 0; i < noCastlingMoves.count; ++i) {
        std::string move = noCastlingMoves.moves[i].toAlgebraic();
        if (move == "e1-g1" || move == "e1-c1") {
            std::cout << "  ERROR: Found castling when it should not be available: " << move << "\n";
            incorrectCastling = true;
        }
    }
    
    if (!incorrectCastling) {
        std::cout << "  Good: No castling moves found (as expected)\n";
    }
    
    std::cout << "\nSpecial move testing complete.\n";
    std::cout << "If issues found above, they likely contribute to the 35-node discrepancy.\n";
    
    return 0;
}
