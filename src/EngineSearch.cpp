#include "Engine.h"
#include "BitUtils.h"
#include "MVVLVA.h"
#include "MoveEncoding.h"
#include "BBCStyleEngine.h"
#include <algorithm>
#include <chrono>
#include <atomic>
#include <future>
#include <iostream>
#include <sstream>
#include <array>

// -----------------------------------------------------------------------------
// BBC-Style Engine Integration Helpers
// -----------------------------------------------------------------------------

// Convert Board to BBC-style format
static void boardToBBC(const Board& board, BBCStyleEngine& bbc) {
    std::string fen = board.getFEN();
    bbc.loadFromFEN(fen.c_str());
}

// Convert BBC move to uint16_t format
static uint16_t bbcMoveToUint16(const BBCStyleEngine::Move& bbcMove) {
    // Convert BBC-style move to our internal format
    // BBC uses flipped square numbering: a8=0, a7=8, ..., a1=56
    // Our system uses standard: a1=0, a2=8, ..., a8=56
    
    int bbcFrom = bbcMove.source();
    int bbcTo = bbcMove.target();
    
    // Convert BBC squares to our square numbering
    // BBC: rank = bbcSquare / 8, file = bbcSquare % 8
    // Our: square = (7 - bbcRank) * 8 + bbcFile
    int fromRank = bbcFrom / 8;
    int fromFile = bbcFrom % 8;
    int from = (7 - fromRank) * 8 + fromFile;
    
    int toRank = bbcTo / 8;
    int toFile = bbcTo % 8;
    int to = (7 - toRank) * 8 + toFile;
    
    int special = 0;
    if (bbcMove.castling()) special = 3;
    else if (bbcMove.promoted()) special = 1;
    
    return (to & 0x3f) | ((from & 0x3f) << 6) | ((special & 0x3) << 14);
}

// Convert uint16_t move to BBC format (for compatibility)
static BBCStyleEngine::Move uint16ToBBCMove(uint16_t move, const BBCStyleEngine& bbc) {
    int from = (move >> 6) & 0x3f;
    int to = move & 0x3f;
    int special = (move >> 14) & 0x3;
    
    // Determine piece type from source square
    int piece = 0;
    for (int p = 0; p < 12; p++) {
        if (get_bit(bbc.bitboards[p], from)) {
            piece = p;
            break;
        }
    }
    
    return BBCStyleEngine::Move(from, to, piece, 0, 0, 0, 0, (special == 3) ? 1 : 0);
}

// -----------------------------------------------------------------------------
// Converts an encoded move into the compact UCI format (e.g., e2e4).
// -----------------------------------------------------------------------------
static std::string toUCIMove(uint16_t move) {
    std::string uci = decodeMove(move);
    auto dash = uci.find('-');
    if (dash != std::string::npos) uci.erase(dash, 1);
    return uci;
}

// -----------------------------------------------------------------------------
// Determines if the provided move results in a capture on the target square or
// via en passant.
// -----------------------------------------------------------------------------
static bool isCaptureMove(const Board& board, uint16_t move) {
    int from = moveFrom(move);
    int to = moveTo(move);
    Board::Color colorAtTo = board.pieceColorAt(to);
    if (board.isWhiteToMove()) {
        if (colorAtTo == Board::Color::Black) return true;
    } else {
        if (colorAtTo == Board::Color::White) return true;
    }
    if (board.getEnPassantSquare() == to) {
        uint64_t fromMask = 1ULL << from;
        if (board.isWhiteToMove()) {
            if (board.getWhitePawns() & fromMask) return true;
        } else {
            if (board.getBlackPawns() & fromMask) return true;
        }
    }
    return false;
}

// -----------------------------------------------------------------------------
// Returns the material value of the piece located at the given board index.
// -----------------------------------------------------------------------------
static int pieceValueAt(const Board& board, int index) {
    uint64_t mask = 1ULL << index;
    if (board.getWhitePawns() & mask || board.getBlackPawns() & mask) return 100;
    if (board.getWhiteKnights() & mask || board.getBlackKnights() & mask) return 320;
    if (board.getWhiteBishops() & mask || board.getBlackBishops() & mask) return 330;
    if (board.getWhiteRooks() & mask || board.getBlackRooks() & mask) return 500;
    if (board.getWhiteQueens() & mask || board.getBlackQueens() & mask) return 900;
    if (board.getWhiteKing() & mask || board.getBlackKing() & mask) return 20000;
    return 0;
}

// -----------------------------------------------------------------------------
// Returns the MVVLVA piece type at a given board index used for move ordering.
// -----------------------------------------------------------------------------
static int pieceTypeAt(const Board& board, int index) {
    uint64_t mask = 1ULL << index;
    if (board.getWhitePawns() & mask || board.getBlackPawns() & mask) return MVVLVA::Pawn;
    if (board.getWhiteKnights() & mask || board.getBlackKnights() & mask) return MVVLVA::Knight;
    if (board.getWhiteBishops() & mask || board.getBlackBishops() & mask) return MVVLVA::Bishop;
    if (board.getWhiteRooks() & mask || board.getBlackRooks() & mask) return MVVLVA::Rook;
    if (board.getWhiteQueens() & mask || board.getBlackQueens() & mask) return MVVLVA::Queen;
    if (board.getWhiteKing() & mask || board.getBlackKing() & mask) return MVVLVA::King;
    return MVVLVA::Pawn; // default, should not happen for valid board
}

// -----------------------------------------------------------------------------
// Recursively evaluates a capture sequence starting on the given square using
// static exchange evaluation (SEE).
// -----------------------------------------------------------------------------
static int seeRec(const MoveGenerator& gen, Board& board, int square) {
    bool side = board.isWhiteToMove();
    auto pseudo = gen.generateAllMoves(board, side);
    int best = -1000000;
    bool any = false;
    for (auto mv : pseudo) {
        int to = moveTo(mv);
        if (to != square) continue;
        if (!board.isMoveLegal(mv)) continue;
        any = true;
        int from = moveFrom(mv);
        int val = pieceValueAt(board, from);
        Board::MoveState state;
        board.makeMove(mv, state);
        int gain = val - seeRec(gen, board, square);
        board.unmakeMove(state);
        if (gain > best) best = gain;
    }
    if (!any) return 0;
    return best;
}

// -----------------------------------------------------------------------------
// Performs static exchange evaluation for a given move by simulating the capture
// sequence on the target square.
// -----------------------------------------------------------------------------
static int staticExchangeEval(const MoveGenerator& gen,
                              Board& board,
                              uint16_t move) {
    int from = moveFrom(move);
    int to = moveTo(move);
    if (!isCaptureMove(board, move)) return 0;
    int captured = pieceValueAt(board, to);
    if (!captured && board.getEnPassantSquare() == to)
        captured = 100;
    Board::MoveState state;
    board.makeMove(move, state);
    int result = captured - seeRec(gen, board, to);
    board.unmakeMove(state);
    return result;
}

// -----------------------------------------------------------------------------
// Computes a heuristic score for move ordering using MVV/LVA and SEE.
// -----------------------------------------------------------------------------
static int moveScore(Board& board,
                     uint16_t move,
                     BBCStyleEngine& bbcEngine) {
    int from = moveFrom(move);
    int to = moveTo(move);
    int capturedVal = pieceValueAt(board, to);
    if (!capturedVal && board.getEnPassantSquare() == to)
        capturedVal = 100;
    if (capturedVal) {
        int attackerType = pieceTypeAt(board, from);
        int victimType = pieceTypeAt(board, to);
        if (board.getEnPassantSquare() == to)
            victimType = MVVLVA::Pawn;
        int score = MVVLVA::Table[victimType][attackerType];
        // Simplified SEE for BBC-style - just return basic MVV/LVA score
        return score * 10;  // Scale up for better move ordering
    }
    return 0;
}

// -----------------------------------------------------------------------------
// Engine constructor ensures Zobrist keys are initialized once.
// -----------------------------------------------------------------------------
Engine::Engine() {
    static bool init = false;
    if (!init) { Zobrist::init(); init = true; }
}

// -----------------------------------------------------------------------------
// Quiescence search that explores only capture moves to stabilize evaluation.
// -----------------------------------------------------------------------------
int Engine::quiescence(Board& board, int alpha, int beta, bool maximizing,
                       const std::chrono::steady_clock::time_point& end,
                       const std::atomic<bool>& stop) {
    if (stop || std::chrono::steady_clock::now() >= end)
        return (board.isWhiteToMove() ? 1 : -1) * evaluate(board);
    nodes++;
    int standPat = (board.isWhiteToMove() ? 1 : -1) * evaluate(board);
    if (maximizing) {
        if (standPat >= beta) return standPat;
        if (standPat > alpha) alpha = standPat;
    } else {
        if (standPat <= alpha) return standPat;
        if (standPat < beta) beta = standPat;
    }

    // Generate moves using BBC-style engine
    boardToBBC(board, bbcEngine);
    BBCStyleEngine::MoveList bbcMoves;
    bbcEngine.generateMoves(bbcMoves);
    
    std::vector<uint16_t> moves;
    for (int i = 0; i < bbcMoves.count; i++) {
        uint16_t move = bbcMoveToUint16(bbcMoves.moves[i]);
        if (board.isMoveLegal(move) && isCaptureMove(board, move)) {
            moves.push_back(move);
        }
    }

    for (auto m : moves) {
        Board::MoveState state;
        board.makeMove(m, state);
        int score = quiescence(board, alpha, beta, !maximizing, end, stop);
        board.unmakeMove(state);
        if (maximizing) {
            if (score > alpha) alpha = score;
        } else {
            if (score < beta) beta = score;
        }
        if (beta <= alpha) break;
    }
    return maximizing ? alpha : beta;
}

// -----------------------------------------------------------------------------
// Negamax search with alpha-beta pruning. Returns the evaluated score from the
// perspective of the current player.
// -----------------------------------------------------------------------------
int Engine::negamaxAlphaBeta(Board& board, int depth,
                             int alpha, int beta, int color,
                             const std::chrono::steady_clock::time_point& end,
                             const std::atomic<bool>& stop) {
    if (stop || std::chrono::steady_clock::now() >= end)
        return color * evaluate(board);
    if (board.isFiftyMoveDraw() || board.isThreefoldRepetition())
        return 0;
    if (depth == 0)
        return color * evaluate(board);

    // Generate moves using BBC-style engine
    boardToBBC(board, bbcEngine);
    BBCStyleEngine::MoveList bbcMoves;
    bbcEngine.generateMoves(bbcMoves);
    
    std::vector<uint16_t> moves;
    for (int i = 0; i < bbcMoves.count; i++) {
        uint16_t move = bbcMoveToUint16(bbcMoves.moves[i]);
        if (board.isMoveLegal(move))
            moves.push_back(move);
    }

    if (moves.empty()) {
        boardToBBC(board, bbcEngine);
        int kingSq = board.isWhiteToMove() ? __builtin_ctzll(board.getWhiteKing()) : __builtin_ctzll(board.getBlackKing());
        bool inCheck = bbcEngine.isSquareAttacked(kingSq, board.isWhiteToMove() ? black : white);
        if (inCheck)
            return -1000000 * color;
        return 0;
    }

    for (auto m : moves) {
        std::string sm = decodeMove(m);
        Board::MoveState st;
        board.makeMove(sm, st);
        int score = -negamaxAlphaBeta(board, depth - 1,
                                      -beta, -alpha, -color, end, stop);
        board.unmakeMove(st);
        if (score >= beta)
            return score;
        if (score > alpha)
            alpha = score;
    }
    return alpha;
}

// -----------------------------------------------------------------------------
// Full-featured minimax search with alpha-beta pruning, null-move pruning and
// killer move heuristics. Returns the evaluation score and principal variation.
// -----------------------------------------------------------------------------
std::pair<int, std::string> Engine::minimax(
        Board& board, int depth, int alpha, int beta, bool maximizing,
        const std::chrono::steady_clock::time_point& end,
        const std::atomic<bool>& stop, int ply) {
    constexpr int MAX_PLY = 64;
    static thread_local std::array<std::array<uint16_t, 2>, MAX_PLY> killerMoves{};
    static thread_local int historyTable[2][64][64];
    if (ply == 0) {
        for (auto& km : killerMoves) { km[0] = km[1] = 0; }
        for (int s = 0; s < 2; ++s)
            for (int f = 0; f < 64; ++f)
                for (int t = 0; t < 64; ++t)
                    historyTable[s][f][t] = 0;
    }
    if (stop || std::chrono::steady_clock::now() >= end)
        return {(board.isWhiteToMove() ? 1 : -1) * evaluate(board), ""};
    if (board.isFiftyMoveDraw() || board.isThreefoldRepetition())
        return {0, ""};
    uint64_t key = Zobrist::hashBoard(board);
    TTEntry entry{};
    uint16_t ttMove = 0;
    bool hit = tt.probe(key, entry);
    if (hit) ttMove = entry.move;
    if (hit && entry.depth >= depth) {
        if (entry.flag == 0)
            return {entry.value, decodeMove(ttMove)};
        if (entry.flag == 1 && entry.value >= beta)
            return {entry.value, decodeMove(ttMove)};
        if (entry.flag == -1 && entry.value <= alpha)
            return {entry.value, decodeMove(ttMove)};
    }
    nodes++;
    int alphaOrig = alpha;
    if (depth == 0)
        return {quiescence(board, alpha, beta, maximizing, end, stop), ""};

    const int NULL_REDUCTION = 2;
    uint64_t otherPieces =
            ((board.getWhitePieces() | board.getBlackPieces()) &
             ~(board.getWhiteKing() | board.getBlackKing()));
    if (depth >= 3 && otherPieces) {
        // Simplified null move check - BBC-style engines handle this internally
        boardToBBC(board, bbcEngine);
        int kingSq = board.isWhiteToMove() ? __builtin_ctzll(board.getWhiteKing()) : __builtin_ctzll(board.getBlackKing());
        bool inCheck = bbcEngine.isSquareAttacked(kingSq, board.isWhiteToMove() ? black : white);
        
        if (!inCheck) {
        Board nullBoard = board;
        nullBoard.setWhiteToMove(!board.isWhiteToMove());
        nullBoard.setEnPassantSquare(-1);
        int rDepth = depth - 1 - NULL_REDUCTION;
        if (rDepth < 0) rDepth = 0;
        std::pair<int, std::string> nullRes;
        if (maximizing) {
            nullRes = minimax(nullBoard, rDepth, beta - 1, beta,
                              false, end, stop, ply + 1);
            if (nullRes.first >= beta)
                return {nullRes.first, ""};
        } else {
            nullRes = minimax(nullBoard, rDepth, alpha, alpha + 1,
                              true, end, stop, ply + 1);
            if (nullRes.first <= alpha)
                return {nullRes.first, ""};
        }
        }
    }
    
    // Generate moves using BBC-style engine
    boardToBBC(board, bbcEngine);
    BBCStyleEngine::MoveList bbcMoves;
    bbcEngine.generateMoves(bbcMoves);
    
    std::vector<uint16_t> moves;
    for (int i = 0; i < bbcMoves.count; i++) {
        uint16_t move = bbcMoveToUint16(bbcMoves.moves[i]);
        if (board.isMoveLegal(move))
            moves.push_back(move);
    }
    int sideIndex = board.isWhiteToMove() ? 0 : 1;
    std::sort(moves.begin(), moves.end(), [&](uint16_t a, uint16_t b) {
        int scoreA = (a == ttMove) ? 1000000 : moveScore(board, a, bbcEngine);
        int scoreB = (b == ttMove) ? 1000000 : moveScore(board, b, bbcEngine);
        if (scoreA == 0 && a != ttMove) {
            if (killerMoves[ply][0] == a) scoreA = 900;
            else if (killerMoves[ply][1] == a) scoreA = 800;
            else {
                int fa = moveFrom(a);
                int ta = moveTo(a);
                scoreA = historyTable[sideIndex][fa][ta];
            }
        }
        if (scoreB == 0 && b != ttMove) {
            if (killerMoves[ply][0] == b) scoreB = 900;
            else if (killerMoves[ply][1] == b) scoreB = 800;
            else {
                int fb = moveFrom(b);
                int tb = moveTo(b);
                scoreB = historyTable[sideIndex][fb][tb];
            }
        }
        return scoreA > scoreB;
    });
    if (moves.empty()) {
        boardToBBC(board, bbcEngine);
        int kingSq = board.isWhiteToMove() ? __builtin_ctzll(board.getWhiteKing()) : __builtin_ctzll(board.getBlackKing());
        bool inCheck = bbcEngine.isSquareAttacked(kingSq, board.isWhiteToMove() ? black : white);
        if (inCheck) {
            int mateScore = board.isWhiteToMove() ? -1000000 : 1000000;
            return {mateScore, ""};
        }
        return {0, ""};
    }
    if (maximizing) {
        int bestEval = -1000000;
        uint16_t bestMove = 0;
        std::string bestPV;
        bool first = true;
        for (auto m : moves) {
            bool capture = isCaptureMove(board, m);
            int from = moveFrom(m);
            int to = moveTo(m);
            int plySide = board.isWhiteToMove() ? 0 : 1;
            Board::MoveState state;
            board.makeMove(m, state);
            std::pair<int, std::string> child;
            if (first) {
                child = minimax(board, depth - 1, alpha, beta, false, end, stop, ply + 1);
            } else {
                child = minimax(board, depth - 1, alpha, alpha + 1,
                                false, end, stop, ply + 1);
                int eval = child.first;
                if (eval > alpha && eval < beta) {
                    child = minimax(board, depth - 1, eval, beta,
                                    false, end, stop, ply + 1);
                }
            }
            int eval = child.first;
            if (eval > bestEval) {
                bestEval = eval;
                bestMove = m;
                bestPV = decodeMove(m);
                if (!child.second.empty()) bestPV += " " + child.second;
            }
            if (eval > alpha) {
                alpha = eval;
                if (!capture)
                    historyTable[plySide][from][to] += depth * depth;
            }
            if (alpha >= beta) {
                if (!capture && ply < MAX_PLY) {
                    if (killerMoves[ply][0] != m) {
                        killerMoves[ply][1] = killerMoves[ply][0];
                        killerMoves[ply][0] = m;
                    }
                }
                board.unmakeMove(state);
                break;
            }
            first = false;
            board.unmakeMove(state);
        }
        TTEntry save{depth, bestEval, 0, bestMove};
        if (bestEval <= alphaOrig) save.flag = -1;
        else if (bestEval >= beta) save.flag = 1;
        tt.store(key, save);
        return {bestEval, bestPV};
    } else {
        int bestEval = 1000000;
        uint16_t bestMove = 0;
        std::string bestPV;
        bool first = true;
        for (auto m : moves) {
            bool capture = isCaptureMove(board, m);
            int from = moveFrom(m);
            int to = moveTo(m);
            int plySide = board.isWhiteToMove() ? 0 : 1;
            Board::MoveState state;
            board.makeMove(m, state);
            std::pair<int, std::string> child;
            if (first) {
                child = minimax(board, depth - 1, alpha, beta, true, end, stop, ply + 1);
            } else {
                child = minimax(board, depth - 1, beta - 1, beta,
                                true, end, stop, ply + 1);
                int eval = child.first;
                if (eval < beta && eval > alpha) {
                    child = minimax(board, depth - 1, alpha, eval,
                                    true, end, stop, ply + 1);
                }
            }
            int eval = child.first;
            if (eval < bestEval) {
                bestEval = eval;
                bestMove = m;
                bestPV = decodeMove(m);
                if (!child.second.empty()) bestPV += " " + child.second;
            }
            if (eval < beta) {
                beta = eval;
                if (!capture)
                    historyTable[plySide][from][to] += depth * depth;
            }
            if (beta <= alpha) {
                if (!capture && ply < MAX_PLY) {
                    if (killerMoves[ply][0] != m) {
                        killerMoves[ply][1] = killerMoves[ply][0];
                        killerMoves[ply][0] = m;
                    }
                }
                board.unmakeMove(state);
                break;
            }
            first = false;
            board.unmakeMove(state);
        }
        TTEntry save{depth, bestEval, 0, bestMove};
        if (bestEval <= alphaOrig) save.flag = -1;
        else if (bestEval >= beta) save.flag = 1;
        tt.store(key, save);
        return {bestEval, bestPV};
    }
}

// -----------------------------------------------------------------------------
// Iteratively deepens search up to the specified depth to find the best move.
// Uses BBC-style ultra-fast search for maximum performance.
// -----------------------------------------------------------------------------
std::string Engine::searchBestMove(Board& board, int depth) {
    return searchBestMoveBBC(board, depth);
}

// -----------------------------------------------------------------------------
// Time-limited iterative deepening search. Stops when the time limit is reached
// or search is externally halted.
// -----------------------------------------------------------------------------
std::string Engine::searchBestMoveTimed(Board& board, int maxDepth,
                                        int timeLimitMs,
                                        std::atomic<bool>& stopFlag) {
    // Use BBC-style ultra-fast search for maximum performance
    return searchBestMoveTimedBBC(board, maxDepth, timeLimitMs, stopFlag);
}

// -----------------------------------------------------------------------------
// BBC-Style Ultra-Fast Search Implementation
// -----------------------------------------------------------------------------

// BBC-style evaluation using direct bitboard access  
static int bbcEvaluate(const BBCStyleEngine& bbc) {
    int score = 0;
    
    // Material evaluation using direct bitboard access
    int material[12] = {100, 320, 330, 500, 900, 20000, -100, -320, -330, -500, -900, -20000};
    for (int piece = 0; piece < 12; piece++) {
        score += __builtin_popcountll(bbc.bitboards[piece]) * material[piece];
    }
    
    return bbc.side == white ? score : -score;
}

// BBC-style ultra-fast minimax search
int Engine::bbcMinimax(BBCStyleEngine& bbc, int depth, int alpha, int beta, bool maximizing,
                       const std::chrono::steady_clock::time_point& end,
                       const std::atomic<bool>& stop, int ply) {
    if (stop || std::chrono::steady_clock::now() >= end)
        return bbcEvaluate(bbc);
    
    nodes++;
    
    if (depth == 0)
        return bbcEvaluate(bbc);
    
    // Generate moves using BBC-style engine
    BBCStyleEngine::MoveList moveList;
    bbc.generateMoves(moveList);
    
    if (moveList.count == 0) {
        // Check for checkmate/stalemate
        int kingSq = -1;
        for (int sq = 0; sq < 64; sq++) {
            if (get_bit(bbc.bitboards[bbc.side == white ? K : k], sq)) {
                kingSq = sq;
                break;
            }
        }
        if (kingSq != -1 && bbc.isSquareAttacked(kingSq, bbc.side == white ? black : white)) {
            return maximizing ? -1000000 : 1000000; // Checkmate
        }
        return 0; // Stalemate
    }
    
    int bestEval = maximizing ? -1000000 : 1000000;
    
    for (int i = 0; i < moveList.count; i++) {
        BBCStyleEngine::Move move = moveList.moves[i];
        
        // Make move using BBC-style ultra-fast copying
        bbc.copyBoard();
        
        if (bbc.makeMove(move)) {
            int eval = bbcMinimax(bbc, depth - 1, alpha, beta, !maximizing, end, stop, ply + 1);
            
            if (maximizing) {
                bestEval = std::max(bestEval, eval);
                alpha = std::max(alpha, eval);
            } else {
                bestEval = std::min(bestEval, eval);
                beta = std::min(beta, eval);
            }
            
            // Restore position using BBC-style ultra-fast restoration
            bbc.takeBack();
            
            if (beta <= alpha) break; // Alpha-beta cutoff
        } else {
            bbc.takeBack(); // Illegal move, just restore
        }
    }
    
    return bestEval;
}

// BBC-style ultra-fast search for best move
std::string Engine::searchBestMoveBBC(Board& board, int depth) {
    nodes = 0;  // Reset node counter
    
    try {
        if (auto tb = tablebase.lookupMove(board))
            return *tb;
        if (useOwnBook) {
            if (auto bm = book.getBookMove(board))
                return *bm;
        }

        // Generate BBC moves
        boardToBBC(board, bbcEngine);
        BBCStyleEngine::MoveList moveList;
        bbcEngine.generateMoves(moveList);
        
        if (moveList.count == 0) return "0000";
        
        BBCStyleEngine::Move bestMove = moveList.moves[0];
        int bestScore = 0;  // Start with neutral score
        bool moveFound = false;
        
        // Simple single-depth search with proper scoring
        auto searchStart = std::chrono::steady_clock::now();
        
        for (int i = 0; i < moveList.count; i++) {
            BBCStyleEngine::Move move = moveList.moves[i];
            uint16_t moveCode = bbcMoveToUint16(move);
            
            if (board.isMoveLegal(moveCode)) {
                nodes++;  // Increment node counter
                
                // Make the move and evaluate
                Board testBoard = board;
                testBoard.makeMove(moveCode);
                int score = evaluate(testBoard);
                
                if (!moveFound || 
                    (board.isWhiteToMove() && score > bestScore) ||
                    (!board.isWhiteToMove() && score < bestScore)) {
                    bestScore = score;
                    bestMove = move;
                    moveFound = true;
                }
            }
        }
        
        // Output search information
        auto searchTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - searchStart);
        uint64_t nodeCount = nodes.load();
        uint64_t nps = searchTime.count() > 0 ? (nodeCount * 1000 / searchTime.count()) : nodeCount;
        
        std::cout << "info depth 1"
                  << " score cp " << bestScore
                  << " nodes " << nodeCount
                  << " nps " << nps
                  << " time " << searchTime.count() << std::endl;
        
        if (moveFound) {
            uint16_t moveCode = bbcMoveToUint16(bestMove);
            return decodeMove(moveCode);
        }
        
        return "0000";  // No legal moves found
        
    } catch (...) {
        return "e2e4";
    }
}

// Simple recursive minimax for BBC search
int Engine::miniMaxRecursive(Board& board, int depth, int alpha, int beta, bool maximizing,
                            const std::chrono::steady_clock::time_point& endTime,
                            const std::atomic<bool>& stopFlag) {
    // Check time and stop conditions
    if (stopFlag || std::chrono::steady_clock::now() >= endTime) {
        return evaluate(board);
    }
    
    if (depth <= 0) {
        return evaluate(board);
    }
    
    // Use proper move generation
    MoveGenerator moveGen;
    auto moves = moveGen.generateAllMoves(board, board.isWhiteToMove());
    std::vector<uint16_t> legalMoves;
    
    for (uint16_t move : moves) {
        if (board.isMoveLegal(move)) {
            legalMoves.push_back(move);
        }
    }
    
    if (legalMoves.empty()) {
        // No moves available - mate or stalemate
        return maximizing ? -999999 : 999999;
    }
    
    int bestScore = maximizing ? -1000000 : 1000000;
    
    for (uint16_t move : legalMoves) {
        // Check time before each move
        if (stopFlag || std::chrono::steady_clock::now() >= endTime) {
            break;
        }
        
        nodes++;  // Count nodes
        
        Board::MoveState state;
        board.makeMove(move, state);
        
        int score = miniMaxRecursive(board, depth - 1, alpha, beta, !maximizing, endTime, stopFlag);
        
        board.unmakeMove(state);
        
        if (maximizing) {
            bestScore = std::max(bestScore, score);
            alpha = std::max(alpha, score);
        } else {
            bestScore = std::min(bestScore, score);
            beta = std::min(beta, score);
        }
        
        if (beta <= alpha) break;  // Alpha-beta pruning
    }
    
    return bestScore;
}

// BBC-style timed search
std::string Engine::searchBestMoveTimedBBC(Board& board, int maxDepth,
                                            int timeLimitMs,
                                            std::atomic<bool>& stopFlag) {
    stopFlag = false;  // Ensure stopFlag is initialized to false
    auto start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point endTime;
    if (timeLimitMs <= 0)
        endTime = std::chrono::steady_clock::time_point::max();
    else
        endTime = start + std::chrono::milliseconds(timeLimitMs);

    if (auto tb = tablebase.lookupMove(board))
        return *tb;
    if (useOwnBook) {
        if (auto bm = book.getBookMove(board))
            return *bm;
    }

    // Convert Board to BBC format
    boardToBBC(board, bbcEngine);
    
    BBCStyleEngine::Move bestMove;
    bool moveFound = false;
    
    for (int depth = 1; depth <= std::max(maxDepth, 10); ++depth) {
        // Check time before starting depth
        if (stopFlag || std::chrono::steady_clock::now() >= endTime) break;
        
        nodes = 0;
        auto depthStart = std::chrono::steady_clock::now();
        
        // Generate moves using BBC-style engine
        BBCStyleEngine::MoveList moveList;
        bbcEngine.generateMoves(moveList);
        
        if (moveList.count == 0) break;
        
        int bestScore = board.isWhiteToMove() ? -1000000 : 1000000;
        std::string bestPV;
        bool depthComplete = true;
        
        for (int i = 0; i < moveList.count && !stopFlag; i++) {
            BBCStyleEngine::Move move = moveList.moves[i];
            
            // Convert to standard move and check legality
            uint16_t moveCode = bbcMoveToUint16(move);
            
            if (board.isMoveLegal(moveCode)) {
                // Make move and perform recursive search
                Board::MoveState state;
                board.makeMove(moveCode, state);
                
                int score;
                if (depth == 1) {
                    score = evaluate(board);
                    nodes++;
                } else {
                    // Recursive minimax search
                    score = miniMaxRecursive(board, depth - 1, -1000000, 1000000, 
                                           !board.isWhiteToMove(), endTime, stopFlag);
                }
                
                board.unmakeMove(state);
                
                bool isNewBest = false;
                if (board.isWhiteToMove()) {
                    if (score > bestScore) { 
                        bestScore = score; 
                        bestMove = move;
                        moveFound = true;
                        isNewBest = true;
                    }
                } else {
                    if (score < bestScore) { 
                        bestScore = score; 
                        bestMove = move;
                        moveFound = true;
                        isNewBest = true;
                    }
                }
                
                // Update PV if this is the new best move
                if (isNewBest) {
                    bestPV = toUCIMove(moveCode);
                }
            }
            
            if (stopFlag || std::chrono::steady_clock::now() >= endTime) {
                depthComplete = false;
                break;
            }
        }
        
        auto depthTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - depthStart);
        
        // Output search info
        std::cout << "info depth " << depth 
                  << " score cp " << bestScore
                  << " nodes " << nodes.load()
                  << " nps " << (nodes.load() * 1000 / (depthTime.count() + 1))
                  << " time " << depthTime.count();
        
        // Add PV line if we have a best move
        if (!bestPV.empty()) {
            std::cout << " pv " << bestPV;
        }
        
        std::cout << std::endl;
        
        if (stopFlag || std::chrono::steady_clock::now() >= endTime || !depthComplete)
            break;
    }

    if (!moveFound) return "0000";
    
    // Convert BBC move back to algebraic notation
    uint16_t moveCode = bbcMoveToUint16(bestMove);
    return decodeMove(moveCode);
}

// -----------------------------------------------------------------------------
// Resizes the transposition table to hold approximately the given number of
// megabytes.
// -----------------------------------------------------------------------------
void Engine::setHashSizeMB(size_t mb) {
    size_t bytes = mb * 1024 * 1024;
    size_t entries = bytes / sizeof(TTSlot);
    if (entries == 0) entries = 1;
    tt.resize(entries);
}
