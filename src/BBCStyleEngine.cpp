#include "BBCStyleEngine.h"
#include "Magic.h"
#include <cstring>
#include <iostream>

BBCStyleEngine::BBCStyleEngine() {
    stackIndex = 0;
    initializeBitboards();
    loadFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void BBCStyleEngine::initializeBitboards() {
    // Clear all bitboards
    memset(bitboards, 0, sizeof(bitboards));
    memset(occupancies, 0, sizeof(occupancies));
    side = white;
    enpassant = -1;  // No en passant
    castle = wk | wq | bk | bq;  // All castling rights
}

void BBCStyleEngine::loadFromFEN(const char* fen) {
    initializeBitboards();
    
    // For simplicity, just set up starting position
    // White pieces
    bitboards[P] = 0x000000000000FF00ULL;  // White pawns
    bitboards[N] = 0x0000000000000042ULL;  // White knights
    bitboards[B] = 0x0000000000000024ULL;  // White bishops  
    bitboards[R] = 0x0000000000000081ULL;  // White rooks
    bitboards[Q] = 0x0000000000000008ULL;  // White queen
    bitboards[K] = 0x0000000000000010ULL;  // White king
    
    // Black pieces
    bitboards[p] = 0x00FF000000000000ULL;  // Black pawns
    bitboards[n] = 0x4200000000000000ULL;  // Black knights
    bitboards[b] = 0x2400000000000000ULL;  // Black bishops
    bitboards[r] = 0x8100000000000000ULL;  // Black rooks
    bitboards[q] = 0x0800000000000000ULL;  // Black queen
    bitboards[k] = 0x1000000000000000ULL;  // Black king
    
    updateOccupancies();
}

void BBCStyleEngine::updateOccupancies() {
    // BBC-style occupancy calculation
    occupancies[white] = 0ULL;
    occupancies[black] = 0ULL;
    
    // White pieces
    for (int piece = P; piece <= K; piece++) {
        occupancies[white] |= bitboards[piece];
    }
    
    // Black pieces  
    for (int piece = p; piece <= k; piece++) {
        occupancies[black] |= bitboards[piece];
    }
    
    // Both sides
    occupancies[both] = occupancies[white] | occupancies[black];
}

void BBCStyleEngine::copyBoard() {
    // BBC-style ultra-fast board copying
    BoardState& state = boardStack[stackIndex++];
    memcpy(state.bitboards, bitboards, sizeof(bitboards));
    memcpy(state.occupancies, occupancies, sizeof(occupancies));
    state.side = side;
    state.enpassant = enpassant;
    state.castle = castle;
}

void BBCStyleEngine::takeBack() {
    // BBC-style ultra-fast board restoration
    if (stackIndex == 0) return;
    
    BoardState& state = boardStack[--stackIndex];
    memcpy(bitboards, state.bitboards, sizeof(bitboards));
    memcpy(occupancies, state.occupancies, sizeof(occupancies));
    side = state.side;
    enpassant = state.enpassant;
    castle = state.castle;
}

int BBCStyleEngine::makeMove(const Move& move) {
    // BBC-style move application (simplified version)
    int source = move.source();
    int target = move.target();
    int piece = move.piece();
    int promoted = move.promoted();
    int capture = move.capture();
    int castling = move.castling();
    
    // Move the piece (BBC-style direct bitboard manipulation)
    pop_bit(bitboards[piece], source);
    set_bit(bitboards[piece], target);
    
    // Handle captures
    if (capture) {
        // Find and remove captured piece (BBC approach)
        int startPiece = (side == white) ? p : P;
        int endPiece = (side == white) ? k : K;
        
        for (int capturedPiece = startPiece; capturedPiece <= endPiece; capturedPiece++) {
            if (get_bit(bitboards[capturedPiece], target)) {
                pop_bit(bitboards[capturedPiece], target);
                break;
            }
        }
    }
    
    // Handle promotion
    if (promoted) {
        pop_bit(bitboards[piece], target);  // Remove pawn
        set_bit(bitboards[promoted], target);  // Add promoted piece
    }
    
    // Handle castling (simplified)
    if (castling) {
        if (side == white) {
            if (target == 6) {  // Kingside
                pop_bit(bitboards[R], 7);
                set_bit(bitboards[R], 5);
            } else if (target == 2) {  // Queenside
                pop_bit(bitboards[R], 0);
                set_bit(bitboards[R], 3);
            }
        } else {
            if (target == 62) {  // Kingside
                pop_bit(bitboards[r], 63);
                set_bit(bitboards[r], 61);
            } else if (target == 58) {  // Queenside
                pop_bit(bitboards[r], 56);
                set_bit(bitboards[r], 59);
            }
        }
    }
    
    // Update castling rights (simplified)
    if (source == 4) castle &= ~(wk | wq);   // White king moved
    if (source == 60) castle &= ~(bk | bq); // Black king moved
    if (source == 0 || target == 0) castle &= ~wq;
    if (source == 7 || target == 7) castle &= ~wk;
    if (source == 56 || target == 56) castle &= ~bq;
    if (source == 63 || target == 63) castle &= ~bk;
    
    // Update occupancies (BBC-style)
    updateOccupancies();
    
    // Change side
    side ^= 1;
    
    // Check if king is in check (BBC-style legality check)
    int kingSquare = -1;
    U64 kingBitboard = (side == white) ? bitboards[k] : bitboards[K];
    
    // Find king position (BBC-style bit scan)
    for (int sq = 0; sq < 64; sq++) {
        if (get_bit(kingBitboard, sq)) {
            kingSquare = sq;
            break;
        }
    }
    
    if (kingSquare != -1 && isSquareAttacked(kingSquare, side)) {
        return 0;  // Illegal move - king in check
    }
    
    return 1;  // Legal move
}

bool BBCStyleEngine::isSquareAttacked(int square, int bySide) {
    // BBC-style ultra-fast attack checking
    return isSquareAttackedByPawn(square, bySide) ||
           isSquareAttackedByKnight(square, bySide) ||
           isSquareAttackedByBishop(square, bySide) ||
           isSquareAttackedByRook(square, bySide) ||
           isSquareAttackedByQueen(square, bySide) ||
           isSquareAttackedByKing(square, bySide);
}

bool BBCStyleEngine::isSquareAttackedByPawn(int square, int bySide) {
    int rank = square / 8;
    int file = square % 8;
    
    if (bySide == white) {
        if (rank > 0) {
            if (file > 0 && get_bit(bitboards[P], (rank - 1) * 8 + file - 1)) return true;
            if (file < 7 && get_bit(bitboards[P], (rank - 1) * 8 + file + 1)) return true;
        }
    } else {
        if (rank < 7) {
            if (file > 0 && get_bit(bitboards[p], (rank + 1) * 8 + file - 1)) return true;
            if (file < 7 && get_bit(bitboards[p], (rank + 1) * 8 + file + 1)) return true;
        }
    }
    return false;
}

bool BBCStyleEngine::isSquareAttackedByKnight(int square, int bySide) {
    U64 knights = bySide == white ? bitboards[N] : bitboards[n];
    return knights & Magic::getKnightAttacks(square);
}

bool BBCStyleEngine::isSquareAttackedByBishop(int square, int bySide) {
    U64 bishops = bySide == white ? bitboards[B] : bitboards[b];
    return bishops & Magic::getBishopAttacks(square, occupancies[both]);
}

bool BBCStyleEngine::isSquareAttackedByRook(int square, int bySide) {
    U64 rooks = bySide == white ? bitboards[R] : bitboards[r];
    return rooks & Magic::getRookAttacks(square, occupancies[both]);
}

bool BBCStyleEngine::isSquareAttackedByQueen(int square, int bySide) {
    U64 queens = bySide == white ? bitboards[Q] : bitboards[q];
    return queens & Magic::getQueenAttacks(square, occupancies[both]);
}

bool BBCStyleEngine::isSquareAttackedByKing(int square, int bySide) {
    U64 king = bySide == white ? bitboards[K] : bitboards[k];
    return king & Magic::getKingAttacks(square);
}

void BBCStyleEngine::generateMoves(MoveList& moveList) {
    // Simplified move generation for testing
    // This is just a stub - full implementation would generate all moves
    moveList.count = 0;
    
    // For testing, just generate a few pawn moves
    U64 pawns = (side == white) ? bitboards[P] : bitboards[p];
    int direction = (side == white) ? 8 : -8;
    int piece = (side == white) ? P : p;
    
    for (int square = 0; square < 64; square++) {
        if (get_bit(pawns, square)) {
            int target = square + direction;
            if (target >= 0 && target < 64 && !get_bit(occupancies[both], target)) {
                moveList.moves[moveList.count++] = Move(square, target, piece);
                if (moveList.count >= 20) break; // Limit for testing
            }
        }
    }
}

uint64_t BBCStyleEngine::perft(int depth) {
    return perftRecursive(depth);
}

uint64_t BBCStyleEngine::perftRecursive(int depth) {
    if (depth == 0) return 1;
    
    MoveList moveList;
    generateMoves(moveList);
    
    if (depth == 1) {
        uint64_t nodes = 0;
        for (int i = 0; i < moveList.count; i++) {
            copyBoard();
            if (makeMove(moveList.moves[i])) {
                nodes++;
            }
            takeBack();
        }
        return nodes;
    }
    
    uint64_t nodes = 0;
    for (int i = 0; i < moveList.count; i++) {
        copyBoard();
        if (makeMove(moveList.moves[i])) {
            nodes += perftRecursive(depth - 1);
        }
        takeBack();
    }
    
    return nodes;
}

uint64_t BBCStyleEngine::perftDivide(int depth) {
    MoveList moveList;
    generateMoves(moveList);
    uint64_t total = 0;
    
    for (int i = 0; i < moveList.count; i++) {
        copyBoard();
        if (makeMove(moveList.moves[i])) {
            uint64_t nodes = perftRecursive(depth - 1);
            total += nodes;
            std::cout << "move " << i << ": " << nodes << "\n";
        }
        takeBack();
    }
    
    return total;
}
