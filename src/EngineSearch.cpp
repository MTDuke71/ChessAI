#include "Engine.h"
#include "BitUtils.h"
#include <algorithm>
#include <chrono>
#include <atomic>
#include <future>
#include <iostream>
#include <sstream>

static constexpr int MAX_PLY = 64;
static std::string killerMoves[MAX_PLY][2];
static int historyTable[64][64];

static void resetHeuristics() {
    for (int i = 0; i < MAX_PLY; ++i) {
        killerMoves[i][0].clear();
        killerMoves[i][1].clear();
    }
    for (int i = 0; i < 64; ++i)
        for (int j = 0; j < 64; ++j)
            historyTable[i][j] = 0;
}

static void storeKiller(int ply, const std::string& move) {
    if (ply >= MAX_PLY) return;
    if (killerMoves[ply][0] != move) {
        killerMoves[ply][1] = killerMoves[ply][0];
        killerMoves[ply][0] = move;
    }
}

static void storeHistory(const std::string& move, int depth) {
    auto dash = move.find('-');
    if (dash == std::string::npos) return;
    int from = algebraicToIndex(move.substr(0, 2));
    int to = algebraicToIndex(move.substr(dash + 1, 2));
    if (from < 0 || to < 0) return;
    historyTable[from][to] += depth * depth;
}

static std::string toUCIMove(const std::string& move) {
    std::string uci = move;
    auto dash = uci.find('-');
    if (dash != std::string::npos) uci.erase(dash, 1);
    return uci;
}

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
        Board copy = board;
        copy.makeMove(mv);
        int gain = val - seeRec(gen, copy, square);
        if (gain > best) best = gain;
    }
    if (!any) return 0;
    return best;
}

static int staticExchangeEval(const MoveGenerator& gen,
                              const Board& board,
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
    Board copy = board;
    copy.makeMove(move);
    return captured - seeRec(gen, copy, to);
}

static int moveScore(const Board& board,
                     const std::string& move,
                     const MoveGenerator& gen) {
    auto dash = move.find('-');
    if (dash == std::string::npos) return 0;
    int from = algebraicToIndex(move.substr(0, 2));
    int to = algebraicToIndex(move.substr(dash + 1, 2));
    if (from < 0 || to < 0) return 0;
    int captured = pieceValueAt(board, to);
    if (!captured && board.getEnPassantSquare() == to)
        captured = 100; // en passant captures a pawn
    if (captured) {
        int attacker = pieceValueAt(board, from);
        int seeVal = staticExchangeEval(gen, board, move);
        return captured * 10 - attacker + seeVal; // MVV/LVA + SEE
    }
    return 0;
}

static int heuristicScore(const Board& board,
                          const std::string& move,
                          const MoveGenerator& gen,
                          int ply) {
    int score = moveScore(board, move, gen);
    if (ply < MAX_PLY) {
        if (move == killerMoves[ply][0]) score += 9000;
        else if (move == killerMoves[ply][1]) score += 8000;
    }
    auto dash = move.find('-');
    if (dash != std::string::npos) {
        int from = algebraicToIndex(move.substr(0, 2));
        int to = algebraicToIndex(move.substr(dash + 1, 2));
        if (from >= 0 && to >= 0)
            score += historyTable[from][to];
    }
    return score;
}

Engine::Engine() {
    static bool init = false;
    if (!init) { Zobrist::init(); init = true; }
}

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
        Board copy = board;
        copy.makeMove(m);
        int score = quiescence(copy, alpha, beta, !maximizing, end, stop);
        if (maximizing) {
            if (score > alpha) alpha = score;
        } else {
            if (score < beta) beta = score;
        }
        if (beta <= alpha) break;
    }
    return maximizing ? alpha : beta;
}

std::pair<int, std::string> Engine::minimax(
        Board& board, int depth, int alpha, int beta, bool maximizing,
        const std::chrono::steady_clock::time_point& end,
        const std::atomic<bool>& stop, int ply) {
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
        return heuristicScore(board, a, generator, ply) >
               heuristicScore(board, b, generator, ply);
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
        int moveCount = 0;
        for (const auto& m : moves) {
            Board copy = board;
            copy.makeMove(m);
            std::pair<int, std::string> child;
            int newDepth = depth - 1;
            bool reduced = false;
            if (depth >= 3 && moveCount >= 3 && !isCaptureMove(board, m) &&
                !generator.isKingInCheck(copy, !board.isWhiteToMove())) {
                newDepth -= 1;
                reduced = true;
            }
            if (first) {
                child = minimax(copy, newDepth, alpha, beta, false, end, stop, ply + 1);
            } else {
                child = minimax(copy, newDepth, alpha, alpha + 1,
                                false, end, stop, ply + 1);
                int eval = child.first;
                if (eval > alpha && eval < beta) {
                    child = minimax(copy, newDepth, eval, beta,
                                    false, end, stop, ply + 1);
                }
            }
            if (reduced && ((child.first > alpha && maximizing) || (child.first < beta && !maximizing))) {
                child = minimax(copy, depth - 1, alpha, beta, false, end, stop, ply + 1);
            }
            int eval = child.first;
            if (eval > bestEval) {
                bestEval = eval;
                bestPV = m;
                if (!child.second.empty()) bestPV += " " + child.second;
            }
            alpha = std::max(alpha, eval);
            if (beta <= alpha) {
                if (!isCaptureMove(board, m))
                    storeKiller(ply, m);
                storeHistory(m, depth);
                break;
            }
            first = false;
            ++moveCount;
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
        int moveCount = 0;
        for (const auto& m : moves) {
            Board copy = board;
            copy.makeMove(m);
            std::pair<int, std::string> child;
            int newDepth = depth - 1;
            bool reduced = false;
            if (depth >= 3 && moveCount >= 3 && !isCaptureMove(board, m) &&
                !generator.isKingInCheck(copy, !board.isWhiteToMove())) {
                newDepth -= 1;
                reduced = true;
            }
            if (first) {
                child = minimax(copy, newDepth, alpha, beta, true, end, stop, ply + 1);
            } else {
                child = minimax(copy, newDepth, beta - 1, beta,
                                true, end, stop, ply + 1);
                int eval = child.first;
                if (eval < beta && eval > alpha) {
                    child = minimax(copy, newDepth, alpha, eval,
                                    true, end, stop, ply + 1);
                }
            }
            if (reduced && ((child.first > alpha && maximizing) || (child.first < beta && !maximizing))) {
                child = minimax(copy, depth - 1, alpha, beta, true, end, stop, ply + 1);
            }
            int eval = child.first;
            if (eval < bestEval) {
                bestEval = eval;
                bestPV = m;
                if (!child.second.empty()) bestPV += " " + child.second;
            }
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                if (!isCaptureMove(board, m))
                    storeKiller(ply, m);
                storeHistory(m, depth);
                break;
            }
            first = false;
            ++moveCount;
        }
        TTEntry save{depth, bestEval, 0};
        if (bestEval <= alphaOrig) save.flag = -1;
        else if (bestEval >= beta) save.flag = 1;
        tt.store(key, save);
        return {bestEval, bestPV};
    }
}

std::string Engine::searchBestMove(Board& board, int depth) {
    if (auto tb = tablebase.lookupMove(board))
        return *tb;
    if (auto bm = book.getBookMove(board))
        return *bm;

    std::atomic<bool> dummyStop(false);
    std::string bestMove;
    resetHeuristics();

    for (int d = 1; d <= depth; ++d) {
        auto pseudoMoves = generator.generateAllMoves(board, board.isWhiteToMove());
        std::vector<std::string> moves;
        for (const auto& mv : pseudoMoves) {
            if (board.isMoveLegal(mv))
                moves.push_back(mv);
        }
        std::sort(moves.begin(), moves.end(), [&](const std::string& a, const std::string& b) {
            return heuristicScore(board, a, generator, 0) >
                   heuristicScore(board, b, generator, 0);
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
                               dummyStop, 1);
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
    if (auto bm = book.getBookMove(board))
        return *bm;

    std::string bestMove;
    std::string bestPV;
    std::string completedMove; // best move from the last fully searched depth
    int bestScore = 0;
    resetHeuristics();
    bool lastDepthComplete = true;
    for (int depth = 1; maxDepth == 0 || depth <= maxDepth; ++depth) {
        nodes = 0;
        auto pseudoMoves = generator.generateAllMoves(board, board.isWhiteToMove());
        std::vector<std::string> moves;
        for (const auto& mv : pseudoMoves) {
            if (board.isMoveLegal(mv))
                moves.push_back(mv);
        }
        std::sort(moves.begin(), moves.end(), [&](const std::string& a, const std::string& b) {
            return heuristicScore(board, a, generator, 0) >
                   heuristicScore(board, b, generator, 0);
        });
        int window = 50;
        int aspAlpha = -1000000;
        int aspBeta = 1000000;
        if (depth > 1) {
            aspAlpha = bestScore - window;
            aspBeta = bestScore + window;
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
                auto res = minimax(copy, depth - 1, aspAlpha, aspBeta,
                                   !board.isWhiteToMove(), endTime, stopFlag, 1);
                if (res.first <= aspAlpha || res.first >= aspBeta) {
                    res = minimax(copy, depth - 1, -1000000, 1000000,
                                  !board.isWhiteToMove(), endTime, stopFlag, 1);
                }
                return res;
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
                          std::chrono::steady_clock::now() - start)
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
        std::cout << "info depth " << depth << " score cp "
                  << (board.isWhiteToMove() ? bestScore : -bestScore)
                  << " nodes " << nodes << " time " << elapsed;
        if (!pvUCI.empty())
            std::cout << " pv " << pvUCI;
        std::cout << '\n';
        if (lastDepthComplete) {
            completedMove = bestMove;
        }
        if (stopFlag || std::chrono::steady_clock::now() >= endTime) break;
    }
    if (!completedMove.empty())
        return completedMove;
    return bestMove;
}
