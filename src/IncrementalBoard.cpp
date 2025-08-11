#include "IncrementalBoard.h"
#include "Magic.h"

IncrementalBoard::IncrementalBoard(const Board& board) {
    // Copy bitboards for fast access
    whitePawns = board.getWhitePawns();
    whiteKnights = board.getWhiteKnights();
    whiteBishops = board.getWhiteBishops();
    whiteRooks = board.getWhiteRooks();
    whiteQueens = board.getWhiteQueens();
    whiteKing = board.getWhiteKing();
    
    blackPawns = board.getBlackPawns();
    blackKnights = board.getBlackKnights();
    blackBishops = board.getBlackBishops();
    blackRooks = board.getBlackRooks();
    blackQueens = board.getBlackQueens();
    blackKing = board.getBlackKing();
    
    whiteToMove = board.isWhiteToMove();
    enPassantSquare = board.getEnPassantSquare();
    castleWK = board.canCastleWK();
    castleWQ = board.canCastleWQ();
    castleBK = board.canCastleBK();
    castleBQ = board.canCastleBQ();
}

uint64_t IncrementalBoard::getWhitePieces() const {
    return whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
}

uint64_t IncrementalBoard::getBlackPieces() const {
    return blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;
}

uint64_t IncrementalBoard::getAllPieces() const {
    return getWhitePieces() | getBlackPieces();
}

// BBC-style ultra-fast legal move checking
bool IncrementalBoard::isMoveLegal(int from, int to, int special, int promotion) {
    MoveState state;
    
    // Apply move incrementally (BBC-style)
    applyMoveIncremental(from, to, special, promotion, state);
    
    // Quick check: Is our king in check? (BBC approach)
    bool legal = !isKingInCheckFast(whiteToMove);
    
    // Rollback move (BBC-style fast restore)
    rollbackMove(state, from, to, special, promotion);
    
    return legal;
}

void IncrementalBoard::applyMoveIncremental(int from, int to, int special, int promotion, MoveState& state) {
    // Save state for rollback
    state.capturedPiece = 0;
    state.capturedSquare = -1;
    state.originalCastleRights = (castleWK ? 1 : 0) | (castleWQ ? 2 : 0) | (castleBK ? 4 : 0) | (castleBQ ? 8 : 0);
    state.originalEnPassant = enPassantSquare;
    state.originalWhiteToMove = whiteToMove;
    
    uint64_t fromMask = 1ULL << from;
    uint64_t toMask = 1ULL << to;
    
    // Find and save captured piece
    uint64_t enemyPieces = whiteToMove ? getBlackPieces() : getWhitePieces();
    if (enemyPieces & toMask) {
        state.capturedSquare = to;
        // Determine captured piece type
        if (whiteToMove) {
            if (blackPawns & toMask) { state.capturedPiece = 1; blackPawns &= ~toMask; }
            else if (blackKnights & toMask) { state.capturedPiece = 2; blackKnights &= ~toMask; }
            else if (blackBishops & toMask) { state.capturedPiece = 3; blackBishops &= ~toMask; }
            else if (blackRooks & toMask) { state.capturedPiece = 4; blackRooks &= ~toMask; }
            else if (blackQueens & toMask) { state.capturedPiece = 5; blackQueens &= ~toMask; }
        } else {
            if (whitePawns & toMask) { state.capturedPiece = 1; whitePawns &= ~toMask; }
            else if (whiteKnights & toMask) { state.capturedPiece = 2; whiteKnights &= ~toMask; }
            else if (whiteBishops & toMask) { state.capturedPiece = 3; whiteBishops &= ~toMask; }
            else if (whiteRooks & toMask) { state.capturedPiece = 4; whiteRooks &= ~toMask; }
            else if (whiteQueens & toMask) { state.capturedPiece = 5; whiteQueens &= ~toMask; }
        }
    }
    
    // Move the piece (BBC-style bit manipulation)
    if (whiteToMove) {
        if (whitePawns & fromMask) { whitePawns &= ~fromMask; whitePawns |= toMask; }
        else if (whiteKnights & fromMask) { whiteKnights &= ~fromMask; whiteKnights |= toMask; }
        else if (whiteBishops & fromMask) { whiteBishops &= ~fromMask; whiteBishops |= toMask; }
        else if (whiteRooks & fromMask) { whiteRooks &= ~fromMask; whiteRooks |= toMask; }
        else if (whiteQueens & fromMask) { whiteQueens &= ~fromMask; whiteQueens |= toMask; }
        else if (whiteKing & fromMask) { whiteKing &= ~fromMask; whiteKing |= toMask; }
    } else {
        if (blackPawns & fromMask) { blackPawns &= ~fromMask; blackPawns |= toMask; }
        else if (blackKnights & fromMask) { blackKnights &= ~fromMask; blackKnights |= toMask; }
        else if (blackBishops & fromMask) { blackBishops &= ~fromMask; blackBishops |= toMask; }
        else if (blackRooks & fromMask) { blackRooks &= ~fromMask; blackRooks |= toMask; }
        else if (blackQueens & fromMask) { blackQueens &= ~fromMask; blackQueens |= toMask; }
        else if (blackKing & fromMask) { blackKing &= ~fromMask; blackKing |= toMask; }
    }
    
    // Handle special moves (castling, en passant, promotion)
    if (special == 3) { // Castling
        if (whiteToMove) {
            if (to == 6) { // Kingside
                whiteRooks &= ~(1ULL << 7);
                whiteRooks |= (1ULL << 5);
            } else if (to == 2) { // Queenside  
                whiteRooks &= ~(1ULL << 0);
                whiteRooks |= (1ULL << 3);
            }
        } else {
            if (to == 62) { // Kingside
                blackRooks &= ~(1ULL << 63);
                blackRooks |= (1ULL << 61);
            } else if (to == 58) { // Queenside
                blackRooks &= ~(1ULL << 56);
                blackRooks |= (1ULL << 59);
            }
        }
    }
    
    // Update castling rights (BBC-style)
    if (from == 4) { castleWK = castleWQ = false; } // White king moved
    if (from == 60) { castleBK = castleBQ = false; } // Black king moved
    if (from == 0 || to == 0) castleWQ = false;
    if (from == 7 || to == 7) castleWK = false;
    if (from == 56 || to == 56) castleBQ = false;
    if (from == 63 || to == 63) castleBK = false;
    
    // Flip side to move
    whiteToMove = !whiteToMove;
}

void IncrementalBoard::rollbackMove(const MoveState& state, int from, int to, int special, int promotion) {
    // Restore side to move first
    whiteToMove = state.originalWhiteToMove;
    
    uint64_t fromMask = 1ULL << from;
    uint64_t toMask = 1ULL << to;
    
    // Move piece back (reverse the move)
    if (whiteToMove) {
        if (whitePawns & toMask) { whitePawns &= ~toMask; whitePawns |= fromMask; }
        else if (whiteKnights & toMask) { whiteKnights &= ~toMask; whiteKnights |= fromMask; }
        else if (whiteBishops & toMask) { whiteBishops &= ~toMask; whiteBishops |= fromMask; }
        else if (whiteRooks & toMask) { whiteRooks &= ~toMask; whiteRooks |= fromMask; }
        else if (whiteQueens & toMask) { whiteQueens &= ~toMask; whiteQueens |= fromMask; }
        else if (whiteKing & toMask) { whiteKing &= ~toMask; whiteKing |= fromMask; }
    } else {
        if (blackPawns & toMask) { blackPawns &= ~toMask; blackPawns |= fromMask; }
        else if (blackKnights & toMask) { blackKnights &= ~toMask; blackKnights |= fromMask; }
        else if (blackBishops & toMask) { blackBishops &= ~toMask; blackBishops |= fromMask; }
        else if (blackRooks & toMask) { blackRooks &= ~toMask; blackRooks |= fromMask; }
        else if (blackQueens & toMask) { blackQueens &= ~toMask; blackQueens |= fromMask; }
        else if (blackKing & toMask) { blackKing &= ~toMask; blackKing |= fromMask; }
    }
    
    // Restore captured piece
    if (state.capturedSquare >= 0) {
        uint64_t capMask = 1ULL << state.capturedSquare;
        if (whiteToMove) { // Enemy was black
            switch (state.capturedPiece) {
                case 1: blackPawns |= capMask; break;
                case 2: blackKnights |= capMask; break;
                case 3: blackBishops |= capMask; break;
                case 4: blackRooks |= capMask; break;
                case 5: blackQueens |= capMask; break;
            }
        } else { // Enemy was white
            switch (state.capturedPiece) {
                case 1: whitePawns |= capMask; break;
                case 2: whiteKnights |= capMask; break;
                case 3: whiteBishops |= capMask; break;
                case 4: whiteRooks |= capMask; break;
                case 5: whiteQueens |= capMask; break;
            }
        }
    }
    
    // Restore castling rights
    castleWK = state.originalCastleRights & 1;
    castleWQ = state.originalCastleRights & 2;
    castleBK = state.originalCastleRights & 4;
    castleBQ = state.originalCastleRights & 8;
    
    // Restore en passant
    enPassantSquare = state.originalEnPassant;
    
    // Handle special move rollbacks
    if (special == 3) { // Castling rollback
        if (whiteToMove) {
            if (to == 6) { // Kingside
                whiteRooks &= ~(1ULL << 5);
                whiteRooks |= (1ULL << 7);
            } else if (to == 2) { // Queenside
                whiteRooks &= ~(1ULL << 3);
                whiteRooks |= (1ULL << 0);
            }
        } else {
            if (to == 62) { // Kingside
                blackRooks &= ~(1ULL << 61);
                blackRooks |= (1ULL << 63);
            } else if (to == 58) { // Queenside
                blackRooks &= ~(1ULL << 59);
                blackRooks |= (1ULL << 56);
            }
        }
    }
}

bool IncrementalBoard::isSquareAttackedFast(int square, bool byWhite) const {
    uint64_t allPieces = getAllPieces();
    
    // Check pawn attacks (BBC-style)
    if (byWhite) {
        int rank = square / 8;
        int file = square % 8;
        if (rank > 0) {
            if (file > 0 && (whitePawns & (1ULL << ((rank - 1) * 8 + file - 1)))) return true;
            if (file < 7 && (whitePawns & (1ULL << ((rank - 1) * 8 + file + 1)))) return true;
        }
    } else {
        int rank = square / 8;
        int file = square % 8;
        if (rank < 7) {
            if (file > 0 && (blackPawns & (1ULL << ((rank + 1) * 8 + file - 1)))) return true;
            if (file < 7 && (blackPawns & (1ULL << ((rank + 1) * 8 + file + 1)))) return true;
        }
    }
    
    // Check knight attacks  
    uint64_t knights = byWhite ? whiteKnights : blackKnights;
    if (knights & Magic::getKnightAttacks(square)) return true;
    
    // Check bishop/queen diagonal attacks
    uint64_t bishops = byWhite ? (whiteBishops | whiteQueens) : (blackBishops | blackQueens);
    if (bishops & Magic::getBishopAttacks(square, allPieces)) return true;
    
    // Check rook/queen orthogonal attacks
    uint64_t rooks = byWhite ? (whiteRooks | whiteQueens) : (blackRooks | blackQueens);
    if (rooks & Magic::getRookAttacks(square, allPieces)) return true;
    
    // Check king attacks
    uint64_t king = byWhite ? whiteKing : blackKing;
    if (king & Magic::getKingAttacks(square)) return true;
    
    return false;
}

bool IncrementalBoard::isKingInCheckFast(bool whiteKing) const {
    uint64_t kingBitboard = whiteKing ? this->whiteKing : this->blackKing;
    if (kingBitboard == 0) return false;
    
    // Find king position (BBC-style bit scan)
    int kingSquare = 0;
    uint64_t temp = kingBitboard;
    while (temp) {
        if (temp & 1) break;
        kingSquare++;
        temp >>= 1;
    }
    
    return isSquareAttackedFast(kingSquare, !whiteKing);
}
