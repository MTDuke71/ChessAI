#include "Engine.h"
#include "BitUtils.h"
#include <algorithm>
#include <chrono>
#include <atomic>
#include <future>
#include <iostream>
#include <sstream>

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

static int moveScore(const Board& board, const std::string& move) {
    auto dash = move.find('-');
    if (dash == std::string::npos) return 0;
    int from = algebraicToIndex(move.substr(0,2));
    int to = algebraicToIndex(move.substr(dash+1,2));
    if (from < 0 || to < 0) return 0;
    int captured = pieceValueAt(board, to);
    if (!captured && board.getEnPassantSquare() == to)
        captured = 100; // en passant captures a pawn
    if (captured) {
        int attacker = pieceValueAt(board, from);
        return captured * 10 - attacker; // MVV/LVA style ordering
    }
    return 0;
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
        if (board.isMoveLegal(mv) && isCaptureMove(board, mv))
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
        const std::atomic<bool>& stop) {
    if (stop || std::chrono::steady_clock::now() >= end)
        return {evaluate(board), ""};
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
    auto pseudoMoves = generator.generateAllMoves(board, board.isWhiteToMove());
    std::vector<std::string> moves;
    for (const auto& mv : pseudoMoves) {
        if (board.isMoveLegal(mv))
            moves.push_back(mv);
    }
    std::sort(moves.begin(), moves.end(), [&](const std::string& a, const std::string& b) {
        return moveScore(board, a) > moveScore(board, b);
    });
    if (moves.empty()) return {evaluate(board), ""};
    if (maximizing) {
        int bestEval = -1000000;
        std::string bestPV;
        bool first = true;
        for (const auto& m : moves) {
            Board copy = board;
            copy.makeMove(m);
            std::pair<int, std::string> child;
            if (first) {
                child = minimax(copy, depth - 1, alpha, beta, false, end, stop);
            } else {
                child = minimax(copy, depth - 1, alpha, alpha + 1,
                                false, end, stop);
                int eval = child.first;
                if (eval > alpha && eval < beta) {
                    child = minimax(copy, depth - 1, eval, beta,
                                    false, end, stop);
                }
            }
            int eval = child.first;
            if (eval > bestEval) {
                bestEval = eval;
                bestPV = m;
                if (!child.second.empty()) bestPV += " " + child.second;
            }
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
            first = false;
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
            Board copy = board;
            copy.makeMove(m);
            std::pair<int, std::string> child;
            if (first) {
                child = minimax(copy, depth - 1, alpha, beta, true, end, stop);
            } else {
                child = minimax(copy, depth - 1, beta - 1, beta,
                                true, end, stop);
                int eval = child.first;
                if (eval < beta && eval > alpha) {
                    child = minimax(copy, depth - 1, alpha, eval,
                                    true, end, stop);
                }
            }
            int eval = child.first;
            if (eval < bestEval) {
                bestEval = eval;
                bestPV = m;
                if (!child.second.empty()) bestPV += " " + child.second;
            }
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
            first = false;
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
    auto pseudoMoves = generator.generateAllMoves(board, board.isWhiteToMove());
    std::vector<std::string> moves;
    for (const auto& mv : pseudoMoves) {
        if (board.isMoveLegal(mv))
            moves.push_back(mv);
    }
    std::sort(moves.begin(), moves.end(), [&](const std::string& a, const std::string& b) {
        return moveScore(board, a) > moveScore(board, b);
    });
    std::string bestMove;
    int bestScore = board.isWhiteToMove() ? -1000000 : 1000000;
    std::atomic<bool> dummyStop(false);

    std::vector<std::future<std::pair<int, std::string>>> futures;
    futures.reserve(moves.size());
    for (const auto& m : moves) {
        futures.emplace_back(std::async(std::launch::async, [&, m]() {
            Board copy = board;
            copy.makeMove(m);
            return minimax(copy, depth - 1, -1000000, 1000000,
                           !board.isWhiteToMove(),
                           std::chrono::steady_clock::time_point::max(),
                           dummyStop);
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
            return moveScore(board, a) > moveScore(board, b);
        });
        bestScore = board.isWhiteToMove() ? -1000000 : 1000000;
        bestPV.clear();
        lastDepthComplete = true;
        std::vector<std::future<std::pair<int, std::string>>> futures;
        futures.reserve(moves.size());
        for (const auto& m : moves) {
            futures.emplace_back(std::async(std::launch::async, [&, m]() {
                Board copy = board;
                copy.makeMove(m);
                return minimax(copy, depth - 1, -1000000, 1000000,
                               !board.isWhiteToMove(), endTime, stopFlag);
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
