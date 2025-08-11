#include "FastMoveGenerator.h"
#include "Magic.h"
#include "BitUtils.h"
#include <cstring>

std::string FastMoveGenerator::Move::toAlgebraic() const {
    std::string result;
    
    // Convert square indices to algebraic notation
    int fromFile = from() % 8;
    int fromRank = from() / 8;
    int toFile = to() % 8;
    int toRank = to() / 8;
    
    result += char('a' + fromFile);
    result += char('1' + fromRank);
    result += '-';
    result += char('a' + toFile);
    result += char('1' + toRank);
    
    // Add promotion suffix if applicable
    if (promotion() > 0) {
        const char promotions[] = {'n', 'b', 'r', 'q'};
        if (promotion() >= 1 && promotion() <= 4) {
            result += promotions[promotion() - 1];
        }
    }
    
    return result;
}

FastMoveGenerator::FastMoveGenerator() {
    initAttackTables();
}

void FastMoveGenerator::initAttackTables() {
    // Initialize knight attack table
    const int knightOffsets[8][2] = {{1, 2}, {2, 1}, {-1, 2}, {-2, 1},
                                     {1, -2}, {2, -1}, {-1, -2}, {-2, -1}};
    
    for (int sq = 0; sq < 64; ++sq) {
        int rank = sq / 8;
        int file = sq % 8;
        
        // Knight attacks
        uint64_t knightMask = 0ULL;
        for (auto& offset : knightOffsets) {
            int newRank = rank + offset[1];
            int newFile = file + offset[0];
            if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
                knightMask |= 1ULL << (newRank * 8 + newFile);
            }
        }
        knightAttacks[sq] = knightMask;
        
        // King attacks
        uint64_t kingMask = 0ULL;
        for (int dr = -1; dr <= 1; ++dr) {
            for (int df = -1; df <= 1; ++df) {
                if (dr == 0 && df == 0) continue;
                int newRank = rank + dr;
                int newFile = file + df;
                if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
                    kingMask |= 1ULL << (newRank * 8 + newFile);
                }
            }
        }
        kingAttacks[sq] = kingMask;
        
        // Pawn attacks
        // White pawn attacks (moving up the board)
        uint64_t whitePawnMask = 0ULL;
        if (rank < 7) { // Not on 8th rank
            if (file > 0) whitePawnMask |= 1ULL << ((rank + 1) * 8 + file - 1); // Left capture
            if (file < 7) whitePawnMask |= 1ULL << ((rank + 1) * 8 + file + 1); // Right capture
        }
        pawnAttacks[0][sq] = whitePawnMask; // White = 0
        
        // Black pawn attacks (moving down the board)
        uint64_t blackPawnMask = 0ULL;
        if (rank > 0) { // Not on 1st rank
            if (file > 0) blackPawnMask |= 1ULL << ((rank - 1) * 8 + file - 1); // Left capture
            if (file < 7) blackPawnMask |= 1ULL << ((rank - 1) * 8 + file + 1); // Right capture
        }
        pawnAttacks[1][sq] = blackPawnMask; // Black = 1
    }
}

void FastMoveGenerator::generateMoves(const Board& board, bool isWhite, MoveList& moveList) const {
    moveList.clear();
    
    generatePawnMoves(board, isWhite, moveList);
    generateKnightMoves(board, isWhite, moveList);
    generateBishopMoves(board, isWhite, moveList);
    generateRookMoves(board, isWhite, moveList);
    generateQueenMoves(board, isWhite, moveList);
    generateKingMoves(board, isWhite, moveList);
    generateCastlingMoves(board, isWhite, moveList);
}

void FastMoveGenerator::generateLegalMoves(const Board& board, bool isWhite, MoveList& moveList) const {
    // Generate pseudo-legal moves first
    generateMoves(board, isWhite, moveList);
    
    // Filter out illegal moves using efficient checks
    MoveList legalMoves;
    
    for (int i = 0; i < moveList.count; ++i) {
        const Move& move = moveList.moves[i];
        
        if (isMoveLegal(board, move, isWhite)) {
            legalMoves.add(move);
        }
    }
    
    moveList = legalMoves;
}

bool FastMoveGenerator::isMoveLegal(const Board& board, const Move& move, bool isWhite) const {
    // Use the existing Board's makeMove mechanism to test legality
    // This is similar to BBC's approach but uses our existing infrastructure
    
    std::string moveStr = move.toAlgebraic();
    
    // Create a copy of the board to test the move
    // Since we can't easily copy Board, we'll use the const_cast approach
    // but be very careful to unmake the move
    Board& mutableBoard = const_cast<Board&>(board);
    
    try {
        Board::MoveState state;
        mutableBoard.makeMove(moveStr, state);
        
        // Check if our king is in check after this move
        bool legal = !isKingInCheck(mutableBoard, isWhite);
        
        // Always unmake the move
        mutableBoard.unmakeMove(state);
        
        return legal;
    } catch (...) {
        // If makeMove throws an exception, the move is illegal
        return false;
    }
}

bool FastMoveGenerator::isSquareAttacked(const Board& board, int square, bool byWhite) const {
    uint64_t allPieces = board.getWhitePieces() | board.getBlackPieces();
    
    // Check pawn attacks (need to reverse the perspective)
    uint64_t pawns = byWhite ? board.getWhitePawns() : board.getBlackPawns();
    uint64_t pawnAttackMask = pawnAttacks[byWhite ? 0 : 1][square]; // Correct perspective 
    if (pawns & pawnAttackMask) return true;
    
    // Check knight attacks
    uint64_t knights = byWhite ? board.getWhiteKnights() : board.getBlackKnights();
    if (knights & knightAttacks[square]) return true;
    
    // Check bishop/queen diagonal attacks
    uint64_t bishops = byWhite ? (board.getWhiteBishops() | board.getWhiteQueens()) : 
                                 (board.getBlackBishops() | board.getBlackQueens());
    if (bishops & Magic::getBishopAttacks(square, allPieces)) return true;
    
    // Check rook/queen orthogonal attacks
    uint64_t rooks = byWhite ? (board.getWhiteRooks() | board.getWhiteQueens()) : 
                               (board.getBlackRooks() | board.getBlackQueens());
    if (rooks & Magic::getRookAttacks(square, allPieces)) return true;
    
    // Check king attacks
    uint64_t king = byWhite ? board.getWhiteKing() : board.getBlackKing();
    if (king & kingAttacks[square]) return true;
    
    return false;
}

bool FastMoveGenerator::isKingInCheck(const Board& board, bool whiteKing) const {
    uint64_t king = whiteKing ? board.getWhiteKing() : board.getBlackKing();
    if (king == 0) return false;
    
    int kingSquare = lsbIndex(king);
    return isSquareAttacked(board, kingSquare, !whiteKing);
}

void FastMoveGenerator::generatePawnMoves(const Board& board, bool isWhite, MoveList& moveList) const {
    uint64_t pawns = isWhite ? board.getWhitePawns() : board.getBlackPawns();
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();
    uint64_t opponentPieces = isWhite ? board.getBlackPieces() : board.getWhitePieces();
    uint64_t emptySquares = ~(board.getWhitePieces() | board.getBlackPieces());
    
    const uint64_t startRank = isWhite ? 0x000000000000FF00ULL : 0x00FF000000000000ULL;
    const uint64_t promRank = isWhite ? 0xFF00000000000000ULL : 0x00000000000000FFULL;
    
    // Single pushes
    uint64_t singlePush;
    if (isWhite) {
        singlePush = (pawns << 8) & emptySquares;
    } else {
        singlePush = (pawns >> 8) & emptySquares;
    }
    
    while (singlePush) {
        int to = lsbIndex(singlePush);
        int from = isWhite ? to - 8 : to + 8;
        
        if ((1ULL << to) & promRank) {
            // Promotions
            moveList.add(Move(from, to, 0, 4, false)); // Queen promotion
            moveList.add(Move(from, to, 0, 3, false)); // Rook promotion
            moveList.add(Move(from, to, 0, 2, false)); // Bishop promotion
            moveList.add(Move(from, to, 0, 1, false)); // Knight promotion
        } else {
            moveList.add(Move(from, to));
        }
        
        popLSB(singlePush);
    }
    
    // Double pushes - must check that BOTH intermediate and final squares are empty
    uint64_t doublePush;
    if (isWhite) {
        // For white: first check single push is possible, then double push
        uint64_t singlePushSquares = (pawns & startRank) << 8;
        uint64_t validSinglePush = singlePushSquares & emptySquares;
        doublePush = (validSinglePush << 8) & emptySquares;
    } else {
        // For black: first check single push is possible, then double push  
        uint64_t singlePushSquares = (pawns & startRank) >> 8;
        uint64_t validSinglePush = singlePushSquares & emptySquares;
        doublePush = (validSinglePush >> 8) & emptySquares;
    }
    
    while (doublePush) {
        int to = lsbIndex(doublePush);
        int from = isWhite ? to - 16 : to + 16;
        moveList.add(Move(from, to, 0, 0, false, true)); // Double pawn push
        popLSB(doublePush);
    }
    
    // Captures
    while (pawns) {
        int from = lsbIndex(pawns);
        uint64_t attacks = pawnAttacks[isWhite ? 0 : 1][from] & opponentPieces;
        
        while (attacks) {
            int to = lsbIndex(attacks);
            
            if ((1ULL << to) & promRank) {
                // Capture promotions
                moveList.add(Move(from, to, 0, 4, true)); // Queen promotion
                moveList.add(Move(from, to, 0, 3, true)); // Rook promotion
                moveList.add(Move(from, to, 0, 2, true)); // Bishop promotion
                moveList.add(Move(from, to, 0, 1, true)); // Knight promotion
            } else {
                moveList.add(Move(from, to, 0, 0, true)); // Regular capture
            }
            
            popLSB(attacks);
        }
        
        popLSB(pawns);
    }
    
    // En passant captures
    int enPassantSquare = board.getEnPassantSquare();
    if (enPassantSquare != -1) {
        pawns = isWhite ? board.getWhitePawns() : board.getBlackPawns();
        while (pawns) {
            int from = lsbIndex(pawns);
            uint64_t enPassantMask = pawnAttacks[isWhite ? 0 : 1][from];
            if (enPassantMask & (1ULL << enPassantSquare)) {
                moveList.add(Move(from, enPassantSquare, 0, 0, true, false, true)); // En passant capture
            }
            popLSB(pawns);
        }
    }
}

void FastMoveGenerator::generateKnightMoves(const Board& board, bool isWhite, MoveList& moveList) const {
    uint64_t knights = isWhite ? board.getWhiteKnights() : board.getBlackKnights();
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();
    uint64_t opponentPieces = isWhite ? board.getBlackPieces() : board.getWhitePieces();
    
    while (knights) {
        int from = lsbIndex(knights);
        uint64_t attacks = knightAttacks[from] & ~ownPieces; // Can't capture own pieces
        
        while (attacks) {
            int to = lsbIndex(attacks);
            bool isCapture = (1ULL << to) & opponentPieces;
            moveList.add(Move(from, to, 0, 0, isCapture));
            popLSB(attacks);
        }
        
        popLSB(knights);
    }
}

void FastMoveGenerator::generateBishopMoves(const Board& board, bool isWhite, MoveList& moveList) const {
    uint64_t bishops = isWhite ? board.getWhiteBishops() : board.getBlackBishops();
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();
    uint64_t opponentPieces = isWhite ? board.getBlackPieces() : board.getWhitePieces();
    uint64_t allPieces = board.getWhitePieces() | board.getBlackPieces();
    
    while (bishops) {
        int from = lsbIndex(bishops);
        uint64_t attacks = Magic::getBishopAttacks(from, allPieces) & ~ownPieces;
        
        while (attacks) {
            int to = lsbIndex(attacks);
            bool isCapture = (1ULL << to) & opponentPieces;
            moveList.add(Move(from, to, 0, 0, isCapture));
            popLSB(attacks);
        }
        
        popLSB(bishops);
    }
}

void FastMoveGenerator::generateRookMoves(const Board& board, bool isWhite, MoveList& moveList) const {
    uint64_t rooks = isWhite ? board.getWhiteRooks() : board.getBlackRooks();
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();
    uint64_t opponentPieces = isWhite ? board.getBlackPieces() : board.getWhitePieces();
    uint64_t allPieces = board.getWhitePieces() | board.getBlackPieces();
    
    while (rooks) {
        int from = lsbIndex(rooks);
        uint64_t attacks = Magic::getRookAttacks(from, allPieces) & ~ownPieces;
        
        while (attacks) {
            int to = lsbIndex(attacks);
            bool isCapture = (1ULL << to) & opponentPieces;
            moveList.add(Move(from, to, 0, 0, isCapture));
            popLSB(attacks);
        }
        
        popLSB(rooks);
    }
}

void FastMoveGenerator::generateQueenMoves(const Board& board, bool isWhite, MoveList& moveList) const {
    uint64_t queens = isWhite ? board.getWhiteQueens() : board.getBlackQueens();
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();
    uint64_t opponentPieces = isWhite ? board.getBlackPieces() : board.getWhitePieces();
    uint64_t allPieces = board.getWhitePieces() | board.getBlackPieces();
    
    while (queens) {
        int from = lsbIndex(queens);
        uint64_t attacks = (Magic::getBishopAttacks(from, allPieces) | 
                           Magic::getRookAttacks(from, allPieces)) & ~ownPieces;
        
        while (attacks) {
            int to = lsbIndex(attacks);
            bool isCapture = (1ULL << to) & opponentPieces;
            moveList.add(Move(from, to, 0, 0, isCapture));
            popLSB(attacks);
        }
        
        popLSB(queens);
    }
}

void FastMoveGenerator::generateKingMoves(const Board& board, bool isWhite, MoveList& moveList) const {
    uint64_t king = isWhite ? board.getWhiteKing() : board.getBlackKing();
    uint64_t ownPieces = isWhite ? board.getWhitePieces() : board.getBlackPieces();
    uint64_t opponentPieces = isWhite ? board.getBlackPieces() : board.getWhitePieces();
    
    if (king) {
        int from = lsbIndex(king);
        uint64_t attacks = kingAttacks[from] & ~ownPieces;
        
        while (attacks) {
            int to = lsbIndex(attacks);
            bool isCapture = (1ULL << to) & opponentPieces;
            moveList.add(Move(from, to, 0, 0, isCapture));
            popLSB(attacks);
        }
    }
}

void FastMoveGenerator::generateCastlingMoves(const Board& board, bool isWhite, MoveList& moveList) const {
    if (isWhite) {
        // White kingside castling
        if (board.canCastleWK()) {
            uint64_t emptySquares = ~(board.getWhitePieces() | board.getBlackPieces());
            uint64_t kingsidePath = 0x60ULL; // f1 and g1
            if ((emptySquares & kingsidePath) == kingsidePath) {
                // Check if squares are not attacked
                if (!isSquareAttacked(board, 4, false) && // e1 (king)
                    !isSquareAttacked(board, 5, false) && // f1
                    !isSquareAttacked(board, 6, false)) { // g1
                    moveList.add(Move(4, 6, 0, 0, false, false, false, true)); // e1-g1
                }
            }
        }
        
        // White queenside castling
        if (board.canCastleWQ()) {
            uint64_t emptySquares = ~(board.getWhitePieces() | board.getBlackPieces());
            uint64_t queensidePath = 0xEULL; // b1, c1, d1
            if ((emptySquares & queensidePath) == queensidePath) {
                if (!isSquareAttacked(board, 4, false) && // e1 (king)
                    !isSquareAttacked(board, 3, false) && // d1
                    !isSquareAttacked(board, 2, false)) { // c1
                    moveList.add(Move(4, 2, 0, 0, false, false, false, true)); // e1-c1
                }
            }
        }
    } else {
        // Black kingside castling
        if (board.canCastleBK()) {
            uint64_t emptySquares = ~(board.getWhitePieces() | board.getBlackPieces());
            uint64_t kingsidePath = 0x6000000000000000ULL; // f8 and g8
            if ((emptySquares & kingsidePath) == kingsidePath) {
                if (!isSquareAttacked(board, 60, true) && // e8 (king)
                    !isSquareAttacked(board, 61, true) && // f8
                    !isSquareAttacked(board, 62, true)) { // g8
                    moveList.add(Move(60, 62, 0, 0, false, false, false, true)); // e8-g8
                }
            }
        }
        
        // Black queenside castling
        if (board.canCastleBQ()) {
            uint64_t emptySquares = ~(board.getWhitePieces() | board.getBlackPieces());
            uint64_t queensidePath = 0xE00000000000000ULL; // b8, c8, d8
            if ((emptySquares & queensidePath) == queensidePath) {
                if (!isSquareAttacked(board, 60, true) && // e8 (king)
                    !isSquareAttacked(board, 59, true) && // d8
                    !isSquareAttacked(board, 58, true)) { // c8
                    moveList.add(Move(60, 58, 0, 0, false, false, false, true)); // e8-c8
                }
            }
        }
    }
}
