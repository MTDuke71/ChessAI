#include "UltraFastPerft.h"
#include "Magic.h"
#include <chrono>
#include <iostream>

uint64_t UltraFastPerft::perft(const Board& board, int depth) {
    BBCStyleEngine engine;
    boardToBBC(board, engine);
    return perftRecursive(engine, depth);
}

uint64_t UltraFastPerft::perftTimed(const Board& board, int depth, double& ms) {
    auto start = std::chrono::steady_clock::now();
    uint64_t nodes = perft(board, depth);
    auto end = std::chrono::steady_clock::now();
    ms = std::chrono::duration<double, std::milli>(end - start).count();
    return nodes;
}

uint64_t UltraFastPerft::perftDivide(const Board& board, int depth) {
    BBCStyleEngine engine;
    boardToBBC(board, engine);
    
    BBCStyleEngine::MoveList moves;
    generateAllMoves(engine, moves);
    
    uint64_t total = 0;
    for (int i = 0; i < moves.count; i++) {
        engine.copyBoard();
        if (engine.makeMove(moves.moves[i])) {
            uint64_t nodes = perftRecursive(engine, depth - 1);
            total += nodes;
            std::cout << "Move " << i << ": " << nodes << "\n";
        }
        engine.takeBack();
    }
    
    return total;
}

void UltraFastPerft::boardToBBC(const Board& board, BBCStyleEngine& engine) {
    // Convert Board representation to BBC-style bitboards
    engine.bitboards[P] = board.getWhitePawns();
    engine.bitboards[N] = board.getWhiteKnights();
    engine.bitboards[B] = board.getWhiteBishops();
    engine.bitboards[R] = board.getWhiteRooks();
    engine.bitboards[Q] = board.getWhiteQueens();
    engine.bitboards[K] = board.getWhiteKing();
    
    engine.bitboards[p] = board.getBlackPawns();
    engine.bitboards[n] = board.getBlackKnights();
    engine.bitboards[b] = board.getBlackBishops();
    engine.bitboards[r] = board.getBlackRooks();
    engine.bitboards[q] = board.getBlackQueens();
    engine.bitboards[k] = board.getBlackKing();
    
    engine.side = board.isWhiteToMove() ? white : black;
    engine.enpassant = board.getEnPassantSquare();
    
    // Convert castling rights
    engine.castle = 0;
    if (board.canCastleWK()) engine.castle |= wk;
    if (board.canCastleWQ()) engine.castle |= wq;
    if (board.canCastleBK()) engine.castle |= bk;
    if (board.canCastleBQ()) engine.castle |= bq;
    
    engine.updateOccupancies();
}

uint64_t UltraFastPerft::perftRecursive(BBCStyleEngine& engine, int depth) {
    if (depth == 0) return 1;
    
    BBCStyleEngine::MoveList moves;
    generateAllMoves(engine, moves);
    
    if (depth == 1) {
        // Optimized depth-1 case - just count legal moves
        uint64_t nodes = 0;
        for (int i = 0; i < moves.count; i++) {
            engine.copyBoard();
            if (engine.makeMove(moves.moves[i])) {
                nodes++;
            }
            engine.takeBack();
        }
        return nodes;
    }
    
    uint64_t nodes = 0;
    for (int i = 0; i < moves.count; i++) {
        engine.copyBoard();
        if (engine.makeMove(moves.moves[i])) {
            nodes += perftRecursive(engine, depth - 1);
        }
        engine.takeBack();
    }
    
    return nodes;
}

void UltraFastPerft::generateAllMoves(BBCStyleEngine& engine, BBCStyleEngine::MoveList& moves) {
    moves.count = 0;
    
    generatePawnMoves(engine, moves);
    generateKnightMoves(engine, moves);
    generateBishopMoves(engine, moves);
    generateRookMoves(engine, moves);
    generateQueenMoves(engine, moves);
    generateKingMoves(engine, moves);
}

void UltraFastPerft::generatePawnMoves(BBCStyleEngine& engine, BBCStyleEngine::MoveList& moves) {
    U64 pawns = engine.bitboards[engine.side == white ? P : p];
    int piece = engine.side == white ? P : p;
    int direction = engine.side == white ? 8 : -8;
    int startRank = engine.side == white ? 1 : 6;
    
    // Single pawn pushes
    for (int square = 0; square < 64; square++) {
        if (get_bit(pawns, square)) {
            int target = square + direction;
            if (target >= 0 && target < 64 && !get_bit(engine.occupancies[both], target)) {
                moves.moves[moves.count++] = BBCStyleEngine::Move(square, target, piece);
                
                // Double pawn push
                if (square / 8 == startRank) {
                    target = square + 2 * direction;
                    if (target >= 0 && target < 64 && !get_bit(engine.occupancies[both], target)) {
                        moves.moves[moves.count++] = BBCStyleEngine::Move(square, target, piece, 0, 0, 1);
                    }
                }
            }
            
            // Pawn captures
            for (int captureDir : {direction + 1, direction - 1}) {
                target = square + captureDir;
                if (target >= 0 && target < 64 && 
                    abs((target % 8) - (square % 8)) == 1 && // Same rank difference
                    get_bit(engine.occupancies[engine.side == white ? black : white], target)) {
                    moves.moves[moves.count++] = BBCStyleEngine::Move(square, target, piece, 0, 1);
                }
            }
        }
    }
}

void UltraFastPerft::generateKnightMoves(BBCStyleEngine& engine, BBCStyleEngine::MoveList& moves) {
    U64 knights = engine.bitboards[engine.side == white ? N : n];
    int piece = engine.side == white ? N : n;
    
    for (int square = 0; square < 64; square++) {
        if (get_bit(knights, square)) {
            U64 attacks = Magic::getKnightAttacks(square);
            attacks &= ~engine.occupancies[engine.side]; // Remove own pieces
            
            for (int target = 0; target < 64; target++) {
                if (get_bit(attacks, target)) {
                    int capture = get_bit(engine.occupancies[engine.side == white ? black : white], target) ? 1 : 0;
                    moves.moves[moves.count++] = BBCStyleEngine::Move(square, target, piece, 0, capture);
                }
            }
        }
    }
}

void UltraFastPerft::generateBishopMoves(BBCStyleEngine& engine, BBCStyleEngine::MoveList& moves) {
    U64 bishops = engine.bitboards[engine.side == white ? B : b];
    int piece = engine.side == white ? B : b;
    
    for (int square = 0; square < 64; square++) {
        if (get_bit(bishops, square)) {
            U64 attacks = Magic::getBishopAttacks(square, engine.occupancies[both]);
            attacks &= ~engine.occupancies[engine.side]; // Remove own pieces
            
            for (int target = 0; target < 64; target++) {
                if (get_bit(attacks, target)) {
                    int capture = get_bit(engine.occupancies[engine.side == white ? black : white], target) ? 1 : 0;
                    moves.moves[moves.count++] = BBCStyleEngine::Move(square, target, piece, 0, capture);
                }
            }
        }
    }
}

void UltraFastPerft::generateRookMoves(BBCStyleEngine& engine, BBCStyleEngine::MoveList& moves) {
    U64 rooks = engine.bitboards[engine.side == white ? R : r];
    int piece = engine.side == white ? R : r;
    
    for (int square = 0; square < 64; square++) {
        if (get_bit(rooks, square)) {
            U64 attacks = Magic::getRookAttacks(square, engine.occupancies[both]);
            attacks &= ~engine.occupancies[engine.side]; // Remove own pieces
            
            for (int target = 0; target < 64; target++) {
                if (get_bit(attacks, target)) {
                    int capture = get_bit(engine.occupancies[engine.side == white ? black : white], target) ? 1 : 0;
                    moves.moves[moves.count++] = BBCStyleEngine::Move(square, target, piece, 0, capture);
                }
            }
        }
    }
}

void UltraFastPerft::generateQueenMoves(BBCStyleEngine& engine, BBCStyleEngine::MoveList& moves) {
    U64 queens = engine.bitboards[engine.side == white ? Q : q];
    int piece = engine.side == white ? Q : q;
    
    for (int square = 0; square < 64; square++) {
        if (get_bit(queens, square)) {
            U64 attacks = Magic::getQueenAttacks(square, engine.occupancies[both]);
            attacks &= ~engine.occupancies[engine.side]; // Remove own pieces
            
            for (int target = 0; target < 64; target++) {
                if (get_bit(attacks, target)) {
                    int capture = get_bit(engine.occupancies[engine.side == white ? black : white], target) ? 1 : 0;
                    moves.moves[moves.count++] = BBCStyleEngine::Move(square, target, piece, 0, capture);
                }
            }
        }
    }
}

void UltraFastPerft::generateKingMoves(BBCStyleEngine& engine, BBCStyleEngine::MoveList& moves) {
    U64 king = engine.bitboards[engine.side == white ? K : k];
    int piece = engine.side == white ? K : k;
    
    for (int square = 0; square < 64; square++) {
        if (get_bit(king, square)) {
            U64 attacks = Magic::getKingAttacks(square);
            attacks &= ~engine.occupancies[engine.side]; // Remove own pieces
            
            for (int target = 0; target < 64; target++) {
                if (get_bit(attacks, target)) {
                    int capture = get_bit(engine.occupancies[engine.side == white ? black : white], target) ? 1 : 0;
                    moves.moves[moves.count++] = BBCStyleEngine::Move(square, target, piece, 0, capture);
                }
            }
            
            // TODO: Add castling moves
        }
    }
}
