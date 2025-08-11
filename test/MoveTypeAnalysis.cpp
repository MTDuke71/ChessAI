#include <iostream>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h"

int main() {
    std::cout << "Analyzing Move Types for Discrepancies\n";
    std::cout << "======================================\n\n";
    
    Board board;
    FastMoveGenerator fastGen;
    MoveGenerator originalGen;
    
    // Test a position known to have en passant possibilities
    std::cout << "1. Testing en passant position:\n";
    Board enPassantTest;
    // After e2-e4 d7-d5 e4-e5 f7-f5 (black f-pawn can be captured en passant)
    Board::MoveState s1, s2, s3, s4;
    enPassantTest.makeMove("e2-e4", s1);
    enPassantTest.makeMove("d7-d5", s2);
    enPassantTest.makeMove("e4-e5", s3);
    enPassantTest.makeMove("f7-f5", s4);
    
    std::cout << "FEN: " << enPassantTest.getFEN() << "\n";
    std::cout << "En passant square: " << enPassantTest.getEnPassantSquare() << "\n";
    
    FastMoveGenerator::MoveList fastMoves;
    fastGen.generateLegalMoves(enPassantTest, true, fastMoves);
    
    std::vector<uint16_t> originalMoves = originalGen.generateLegalMoves(enPassantTest, true);
    
    std::cout << "FastMoveGenerator: " << fastMoves.count << " moves\n";
    std::cout << "Original: " << originalMoves.size() << " moves\n";
    
    if (fastMoves.count != originalMoves.size()) {
        std::cout << "*** FOUND DISCREPANCY IN EN PASSANT POSITION! ***\n";
        
        // Show all moves from FastMoveGenerator
        std::cout << "FastMoveGenerator moves:\n";
        for (int i = 0; i < fastMoves.count; ++i) {
            std::cout << "  " << fastMoves.moves[i].toAlgebraic() << "\n";
        }
        
        // Show all moves from Original
        std::cout << "Original moves:\n";
        for (const auto& move : originalMoves) {
            std::cout << "  " << indexToAlgebraic(move) << "\n";
        }
    }
    
    std::cout << "\n2. Testing castling position:\n";
    Board castlingTest;
    castlingTest.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    
    FastMoveGenerator::MoveList castlingFast;
    fastGen.generateLegalMoves(castlingTest, true, castlingFast);
    
    std::vector<uint16_t> castlingOrig = originalGen.generateLegalMoves(castlingTest, true);
    
    std::cout << "FEN: " << castlingTest.getFEN() << "\n";
    std::cout << "FastMoveGenerator: " << castlingFast.count << " moves\n";
    std::cout << "Original: " << castlingOrig.size() << " moves\n";
    
    if (castlingFast.count != castlingOrig.size()) {
        std::cout << "*** FOUND DISCREPANCY IN CASTLING POSITION! ***\n";
    }
    
    std::cout << "\n3. Testing pawn promotion position:\n";
    Board promotionTest;
    promotionTest.loadFEN("8/P7/8/8/8/8/8/8 w - - 0 1"); // White pawn about to promote
    
    FastMoveGenerator::MoveList promotionFast;
    fastGen.generateLegalMoves(promotionTest, true, promotionFast);
    
    std::vector<uint16_t> promotionOrig = originalGen.generateLegalMoves(promotionTest, true);
    
    std::cout << "FEN: " << promotionTest.getFEN() << "\n";
    std::cout << "FastMoveGenerator: " << promotionFast.count << " moves\n";
    std::cout << "Original: " << promotionOrig.size() << " moves\n";
    
    if (promotionFast.count != promotionOrig.size()) {
        std::cout << "*** FOUND DISCREPANCY IN PROMOTION POSITION! ***\n";
        
        std::cout << "FastMoveGenerator moves:\n";
        for (int i = 0; i < promotionFast.count; ++i) {
            std::cout << "  " << promotionFast.moves[i].toAlgebraic() << "\n";
        }
        
        std::cout << "Original moves:\n";
        for (const auto& move : promotionOrig) {
            std::cout << "  " << indexToAlgebraic(move) << "\n";
        }
    }
    
    return 0;
}
