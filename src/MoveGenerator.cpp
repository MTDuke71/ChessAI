#include "MoveGenerator.h"
#include "MoveEncoding.h"

MoveGenerator::MoveGenerator() {
    // FastMoveGenerator constructor handles initialization
}

std::vector<uint16_t> MoveGenerator::convertMoves(const FastMoveGenerator::MoveList& moveList) const {
    std::vector<uint16_t> result;
    result.reserve(moveList.count);
    
    for (int i = 0; i < moveList.count; i++) {
        const auto& move = moveList.moves[i];
        
        // Convert FastMoveGenerator::Move to uint16_t using the standard encoding
        std::string algebraic = move.toAlgebraic();
        uint16_t encoded = encodeMove(algebraic);
        result.push_back(encoded);
    }
    
    return result;
}

std::vector<uint16_t> MoveGenerator::generateLegalMoves(const Board& board, bool isWhite) const {
    FastMoveGenerator::MoveList moveList;
    getFastGenerator().generateLegalMoves(board, isWhite, moveList);
    return convertMoves(moveList);
}

std::vector<uint16_t> MoveGenerator::generateAllMoves(const Board& board, bool isWhite) const {
    FastMoveGenerator::MoveList moveList;
    getFastGenerator().generateMoves(board, isWhite, moveList);
    return convertMoves(moveList);
}

std::vector<uint16_t> MoveGenerator::generatePawnMoves(const Board& board, bool isWhite) const {
    FastMoveGenerator::MoveList moveList;
    getFastGenerator().generatePawnMoves(board, isWhite, moveList);
    return convertMoves(moveList);
}

std::vector<uint16_t> MoveGenerator::generateKnightMoves(const Board& board, bool isWhite) const {
    FastMoveGenerator::MoveList moveList;
    getFastGenerator().generateKnightMoves(board, isWhite, moveList);
    return convertMoves(moveList);
}

std::vector<uint16_t> MoveGenerator::generateRookMoves(const Board& board, bool isWhite) const {
    FastMoveGenerator::MoveList moveList;
    getFastGenerator().generateRookMoves(board, isWhite, moveList);
    return convertMoves(moveList);
}

std::vector<uint16_t> MoveGenerator::generateBishopMoves(const Board& board, bool isWhite) const {
    FastMoveGenerator::MoveList moveList;
    getFastGenerator().generateBishopMoves(board, isWhite, moveList);
    return convertMoves(moveList);
}

std::vector<uint16_t> MoveGenerator::generateQueenMoves(const Board& board, bool isWhite) const {
    FastMoveGenerator::MoveList moveList;
    getFastGenerator().generateQueenMoves(board, isWhite, moveList);
    return convertMoves(moveList);
}

std::vector<uint16_t> MoveGenerator::generateKingMoves(const Board& board, bool isWhite) const {
    FastMoveGenerator::MoveList moveList;
    getFastGenerator().generateKingMoves(board, isWhite, moveList);
    return convertMoves(moveList);
}

bool MoveGenerator::isSquareAttacked(const Board& board, int square, bool byWhite) const {
    return getFastGenerator().isSquareAttacked(board, square, byWhite);
}

bool MoveGenerator::isKingInCheck(const Board& board, bool white) const {
    return getFastGenerator().isKingInCheck(board, white);
}

void MoveGenerator::addMoves(std::vector<uint16_t>& moves, uint64_t pawns, uint64_t moveBoard, int shift) const {
    // This legacy function is not needed with FastMoveGenerator
    // Keep empty implementation for compatibility
}
