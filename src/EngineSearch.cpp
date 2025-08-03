#include "Engine.h"
#include "BitUtils.h"
#include "MVVLVA.h"
#include <algorithm>
#include <chrono>
#include <atomic>
#include <future>
#include <iostream>
#include <sstream>
#include <array>

// -----------------------------------------------------------------------------
// Converts an internal move representation (e.g., "e2-e4") into the compact UCI
// format ("e2e4").
// -----------------------------------------------------------------------------
static std::string toUCIMove(const std::string& move) {
    std::string uci = move;
    auto dash = uci.find('-');
    if (dash != std::string::npos) uci.erase(dash, 1);
    return uci;
}

// -----------------------------------------------------------------------------
// Determines if the provided move results in a capture on the target square or
// via en passant.
// -----------------------------------------------------------------------------
static bool isCaptureMove(const Board& board, const std::string& move) {
    auto dash = move.find('-');
    if (dash == std::string::npos) return false;
    int from = algebraicToIndex(move.substr(0,2));
    int to = algebraicToIndex(move.substr(dash+1,2));
    if (from < 0 || to < 0) return false;
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
    for (const auto& mv : pseudo) {
        auto dash = mv.find('-');
        if (dash == std::string::npos) continue;
        int to = algebraicToIndex(mv.substr(dash + 1, 2));
        if (to != square) continue;
        if (!board.isMoveLegal(mv)) continue;
        any = true;
        int from = algebraicToIndex(mv.substr(0, 2));
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
                              const std::string& move) {
    auto dash = move.find('-');
    if (dash == std::string::npos) return 0;
    int from = algebraicToIndex(move.substr(0, 2));
    int to = algebraicToIndex(move.substr(dash + 1, 2));
    if (from < 0 || to < 0) return 0;
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
                     const std::string& move,
                     const MoveGenerator& gen) {
    auto dash = move.find('-');
    if (dash == std::string::npos) return 0;
    int from = algebraicToIndex(move.substr(0, 2));
    int to = algebraicToIndex(move.substr(dash + 1, 2));
    if (from < 0 || to < 0) return 0;
    int capturedVal = pieceValueAt(board, to);
    if (!capturedVal && board.getEnPassantSquare() == to)
        capturedVal = 100; // en passant captures a pawn
    if (capturedVal) {
        int attackerType = pieceTypeAt(board, from);
        int victimType = pieceTypeAt(board, to);
        if (board.getEnPassantSquare() == to)
            victimType = MVVLVA::Pawn;
        int score = MVVLVA::Table[victimType][attackerType];
        int seeVal = staticExchangeEval(gen, board, move);
        return score * 10 + seeVal; // MVV/LVA table + SEE
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
        return evaluate(board);
    nodes++;
    int standPat = evaluate(board);
    if (maximizing) {
        if (standPat >= beta) return standPat;
        if (standPat > alpha) alpha = standPat;
    } else {
        if (standPat <= alpha) return standPat;
        if (standPat < beta) beta = standPat;
    }

    auto pseudoMoves = generator.generateAllMoves(board, board.isWhiteToMove());
    std::vector<std::string> moves;
    for (const auto& mv : pseudoMoves)
        if (board.isMoveLegal(mv) && isCaptureMove(board, mv) &&
            staticExchangeEval(generator, board, mv) >= 0)
            moves.push_back(mv);

    for (const auto& m : moves) {
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

    auto pseudo = generator.generateAllMoves(board, board.isWhiteToMove());
    std::vector<std::string> moves;
    for (const auto& mv : pseudo)
        if (board.isMoveLegal(mv))
            moves.push_back(mv);

    if (moves.empty()) {
        if (generator.isKingInCheck(board, board.isWhiteToMove()))
            return -1000000 * color;
        return 0;
    }

    for (const auto& m : moves) {
        Board::MoveState st;
        board.makeMove(m, st);
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
    static thread_local std::array<std::array<std::string, 2>, MAX_PLY> killerMoves;
    if (ply == 0) {
        for (auto& km : killerMoves) { km[0].clear(); km[1].clear(); }
    }
    if (stop || std::chrono::steady_clock::now() >= end)
        return {evaluate(board), ""};
    if (board.isFiftyMoveDraw() || board.isThreefoldRepetition())
        return {0, ""};
    uint64_t key = Zobrist::hashBoard(board);
    TTEntry entry;
    if (tt.probe(key, entry) && entry.depth >= depth) {
        if (entry.flag == 0)
            return {entry.value, ""};
        if (entry.flag == 1 && entry.value >= beta)
            return {entry.value, ""};
        if (entry.flag == -1 && entry.value <= alpha)
            return {entry.value, ""};
    }
    nodes++;
    int alphaOrig = alpha;
    if (depth == 0)
        return {quiescence(board, alpha, beta, maximizing, end, stop), ""};

    const int NULL_REDUCTION = 2;
    uint64_t otherPieces =
            ((board.getWhitePieces() | board.getBlackPieces()) &
             ~(board.getWhiteKing() | board.getBlackKing()));
    if (depth >= 3 && otherPieces &&
        !generator.isKingInCheck(board, board.isWhiteToMove())) {
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
    auto pseudoMoves = generator.generateAllMoves(board, board.isWhiteToMove());
    std::vector<std::string> moves;
    for (const auto& mv : pseudoMoves) {
        if (board.isMoveLegal(mv))
            moves.push_back(mv);
    }
    std::sort(moves.begin(), moves.end(), [&](const std::string& a, const std::string& b) {
        int scoreA = moveScore(board, a, generator);
        int scoreB = moveScore(board, b, generator);
        if (scoreA == 0) {
            if (killerMoves[ply][0] == a) scoreA = 900;
            else if (killerMoves[ply][1] == a) scoreA = 800;
        }
        if (scoreB == 0) {
            if (killerMoves[ply][0] == b) scoreB = 900;
            else if (killerMoves[ply][1] == b) scoreB = 800;
        }
        return scoreA > scoreB;
    });
    if (moves.empty()) {
        if (generator.isKingInCheck(board, board.isWhiteToMove())) {
            int mateScore = board.isWhiteToMove() ? -1000000 : 1000000;
            return {mateScore, ""};
        }
        return {0, ""}; // stalemate
    }
    if (maximizing) {
        int bestEval = -1000000;
        std::string bestPV;
        bool first = true;
        for (const auto& m : moves) {
            bool capture = isCaptureMove(board, m);
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
                bestPV = m;
                if (!child.second.empty()) bestPV += " " + child.second;
            }
            if (eval > alpha) alpha = eval;
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
        TTEntry save{depth, bestEval, 0};
        if (bestEval <= alphaOrig) save.flag = -1;
        else if (bestEval >= beta) save.flag = 1;
        tt.store(key, save);
        return {bestEval, bestPV};
    } else {
        int bestEval = 1000000;
        std::string bestPV;
        bool first = true;
        for (const auto& m : moves) {
            bool capture = isCaptureMove(board, m);
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
                bestPV = m;
                if (!child.second.empty()) bestPV += " " + child.second;
            }
            if (eval < beta) beta = eval;
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
        TTEntry save{depth, bestEval, 0};
        if (bestEval <= alphaOrig) save.flag = -1;
        else if (bestEval >= beta) save.flag = 1;
        tt.store(key, save);
        return {bestEval, bestPV};
    }
}

// -----------------------------------------------------------------------------
// Iteratively deepens search up to the specified depth to find the best move.
// -----------------------------------------------------------------------------
std::string Engine::searchBestMove(Board& board, int depth) {
    if (auto tb = tablebase.lookupMove(board))
        return *tb;
    if (useOwnBook) {
        if (auto bm = book.getBookMove(board))
            return *bm;
    }

    std::atomic<bool> dummyStop(false);
    std::string bestMove;

    for (int d = 1; d <= depth; ++d) {
        auto pseudoMoves = generator.generateAllMoves(board, board.isWhiteToMove());
        std::vector<std::string> moves;
        for (const auto& mv : pseudoMoves) {
            if (board.isMoveLegal(mv))
                moves.push_back(mv);
        }
        std::sort(moves.begin(), moves.end(), [&](const std::string& a, const std::string& b) {
            return moveScore(board, a, generator) > moveScore(board, b, generator);
        });
        if (d > 1 && !bestMove.empty()) {
            auto it = std::find(moves.begin(), moves.end(), bestMove);
            if (it != moves.end()) {
                std::rotate(moves.begin(), it, it + 1);
            }
        }

        int bestScore = board.isWhiteToMove() ? -1000000 : 1000000;
        std::vector<std::future<std::pair<int, std::string>>> futures;
        futures.reserve(moves.size());
        for (const auto& m : moves) {
            futures.emplace_back(pool.enqueue([&, m, d]() {
                Board copy = board;
                copy.makeMove(m);
                return minimax(copy, d - 1, -1000000, 1000000,
                               !board.isWhiteToMove(),
                               std::chrono::steady_clock::time_point::max(),
                               dummyStop, 0);
            }));
        }

        for (size_t i = 0; i < moves.size(); ++i) {
            auto res = futures[i].get();
            int score = res.first;
            const auto& m = moves[i];
            if (board.isWhiteToMove()) {
                if (score > bestScore) { bestScore = score; bestMove = m; }
            } else {
                if (score < bestScore) { bestScore = score; bestMove = m; }
            }
        }
    }

    return bestMove;
}

// -----------------------------------------------------------------------------
// Time-limited iterative deepening search. Stops when the time limit is reached
// or search is externally halted.
// -----------------------------------------------------------------------------
std::string Engine::searchBestMoveTimed(Board& board, int maxDepth,
                                        int timeLimitMs,
                                        std::atomic<bool>& stopFlag) {
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

    std::string bestMove;
    std::string bestPV;
    std::string completedMove; // best move from the last fully searched depth
    int bestScore = 0;
    bool lastDepthComplete = true;
    for (int depth = 1; maxDepth == 0 || depth <= maxDepth; ++depth) {
        nodes = 0;
        auto depthStart = std::chrono::steady_clock::now();
        auto pseudoMoves = generator.generateAllMoves(board, board.isWhiteToMove());
        std::vector<std::string> moves;
        for (const auto& mv : pseudoMoves) {
            if (board.isMoveLegal(mv))
                moves.push_back(mv);
        }
        std::sort(moves.begin(), moves.end(), [&](const std::string& a, const std::string& b) {
            return moveScore(board, a, generator) > moveScore(board, b, generator);
        });
        if (depth > 1 && !completedMove.empty()) {
            auto it = std::find(moves.begin(), moves.end(), completedMove);
            if (it != moves.end()) {
                std::rotate(moves.begin(), it, it + 1);
            }
        }
        bestScore = board.isWhiteToMove() ? -1000000 : 1000000;
        bestPV.clear();
        lastDepthComplete = true;
        std::vector<std::future<std::pair<int, std::string>>> futures;
        futures.reserve(moves.size());
        for (const auto& m : moves) {
            futures.emplace_back(pool.enqueue([&, m]() {
                Board copy = board;
                copy.makeMove(m);
                return minimax(copy, depth - 1, -1000000, 1000000,
                               !board.isWhiteToMove(), endTime, stopFlag, 0);
            }));
        }
        for (size_t i = 0; i < moves.size(); ++i) {
            auto res = futures[i].get();
            const auto& m = moves[i];
            int score = res.first;
            std::string pvCandidate = m;
            if (!res.second.empty()) pvCandidate += " " + res.second;
            if (board.isWhiteToMove()) {
                if (score > bestScore) { bestScore = score; bestMove = m; bestPV = pvCandidate; }
            } else {
                if (score < bestScore) { bestScore = score; bestMove = m; bestPV = pvCandidate; }
            }
            if (stopFlag || std::chrono::steady_clock::now() >= endTime)
                lastDepthComplete = false;
        }
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::steady_clock::now() - depthStart)
                          .count();
        std::string pvUCI;
        {
            std::istringstream iss(bestPV);
            std::string token;
            while (iss >> token) {
                if (!pvUCI.empty()) pvUCI += " ";
                pvUCI += toUCIMove(token);
            }
        }
        int hashPercent = static_cast<int>(tt.used() * 1000 / tt.size());
        uint64_t nodeCount = nodes.load();
        uint64_t nps = elapsed > 0 ? (nodeCount * 1000 / elapsed) : nodeCount;
        if (!lastDepthComplete) break;
        std::cout << "info depth " << depth << " score cp "
                  << (board.isWhiteToMove() ? bestScore : -bestScore)
                  << " nodes " << nodeCount << " nps " << nps
                  << " hashfull " << hashPercent << " time " << elapsed;
        if (!pvUCI.empty())
            std::cout << " pv " << pvUCI;
        std::cout << '\n';
        completedMove = bestMove;
        if (stopFlag || std::chrono::steady_clock::now() >= endTime) break;
    }
    if (!completedMove.empty())
        return completedMove;
    return bestMove;
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
