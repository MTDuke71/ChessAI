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
        
        // Direct conversion from FastMoveGenerator::Move to uint16_t
        // This eliminates the expensive string conversion bottleneck
        uint16_t encoded = convertMoveDirect(move);
        result.push_back(encoded);
    }
    
    return result;
}

uint16_t MoveGenerator::convertMoveDirect(const FastMoveGenerator::Move& move) const {
    // Direct conversion from FastMoveGenerator::Move to uint16_t encoding
    // This bypasses the expensive string conversion bottleneck
    
    int from = move.from();
    int to = move.to();
    uint16_t encoded = (to & 0x3f) | ((from & 0x3f) << 6);
    
    int special = 0;
    
    if (move.isCastling()) {
        special = 3; // Castling flag
    } else if (move.promotion() != 0) {
        special = 1; // Promotion flag
        // Convert piece type to promotion bits
        int promoBits = 0;
        switch (move.promotion()) {
            case 1: promoBits = 0; break; // Knight
            case 2: promoBits = 1; break; // Bishop  
            case 3: promoBits = 2; break; // Rook
            case 4: promoBits = 3; break; // Queen
            default: promoBits = 3; break; // Default to queen
        }
        encoded |= (promoBits & 0x3) << 12;
    }
    // Note: En passant is handled as a regular move in this encoding
    
    encoded |= (special & 0x3) << 14;
    return encoded;
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
